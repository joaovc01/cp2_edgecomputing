#define LED_R 25
#define LED_G 26
#define LED_B 27

const uint8_t NUM_LEITURAS = 5;
const unsigned long INTERVALO_LEITURA_MS = 2000;
const unsigned long INTERVALO_SESSAO_MS = 48000;

const float LIMITE_ALERTA = 16.0;
const float LIMITE_NORMAL = 14.0;

int leituras[NUM_LEITURAS];
uint8_t indiceLeitura = 0;
unsigned long inicioSessaoMillis = 0;
unsigned long ultimaLeituraMillis = 0;
bool sessaoCompleta = false;

enum EstadoVegetacao { NORMAL, ALERTA };
EstadoVegetacao estadoAtual = NORMAL;

void definirLed(bool r, bool g, bool b) {
  digitalWrite(LED_R, r ? HIGH : LOW);
  digitalWrite(LED_G, g ? HIGH : LOW);
  digitalWrite(LED_B, b ? HIGH : LOW);
}

void atualizarLedEstado() {
  if (estadoAtual == ALERTA) {
    definirLed(true, false, false);
  } else {
    definirLed(false, true, false);
  }
}

int gerarLeitura() {
  return random(10, 21);
}

void iniciarSessao() {
  Serial.println("========================================");
  Serial.println("MONITORAMENTO DE VEGETACAO - FW 2.0");
  Serial.println("========================================");
  indiceLeitura = 0;
  sessaoCompleta = false;
  inicioSessaoMillis = millis();
  ultimaLeituraMillis = inicioSessaoMillis - INTERVALO_LEITURA_MS;
}

void registrarLeitura() {
  int valor = gerarLeitura();
  leituras[indiceLeitura] = valor;
  Serial.print("Leitura ");
  Serial.print(indiceLeitura + 1);
  Serial.print(": ");
  Serial.print(valor);
  Serial.println(" cm");
  indiceLeitura++;
}

void ordenarVetor(int origem[], int destino[]) {
  for (uint8_t i = 0; i < NUM_LEITURAS; i++) {
    destino[i] = origem[i];
  }
  for (uint8_t i = 0; i < NUM_LEITURAS - 1; i++) {
    for (uint8_t j = 0; j < NUM_LEITURAS - 1 - i; j++) {
      if (destino[j] > destino[j + 1]) {
        int temp = destino[j];
        destino[j] = destino[j + 1];
        destino[j + 1] = temp;
      }
    }
  }
}

void imprimirVetor(const char* rotulo, int vetor[]) {
  Serial.print(rotulo);
  for (uint8_t i = 0; i < NUM_LEITURAS; i++) {
    Serial.print(vetor[i]);
    Serial.print(" ");
  }
  Serial.println();
}

void atualizarEstado(float mediana) {
  if (mediana >= LIMITE_ALERTA) {
    estadoAtual = ALERTA;
  } else if (mediana <= LIMITE_NORMAL) {
    estadoAtual = NORMAL;
  }
  atualizarLedEstado();
}

void finalizarSessao() {
  float soma = 0;
  for (uint8_t i = 0; i < NUM_LEITURAS; i++) {
    soma += leituras[i];
  }
  float media = soma / NUM_LEITURAS;

  int ordenado[NUM_LEITURAS];
  ordenarVetor(leituras, ordenado);
  int mediana = ordenado[2];

  imprimirVetor("Ordem original: ", leituras);
  imprimirVetor("Ordem crescente: ", ordenado);
  Serial.print("Media da sessao: ");
  Serial.print(media, 1);
  Serial.println(" cm");
  Serial.print("Mediana da sessao: ");
  Serial.print(mediana);
  Serial.println(" cm");

  atualizarEstado((float)mediana);
  Serial.print("Estado: ");
  Serial.println(estadoAtual == ALERTA ? "ALERTA" : "NORMAL");

  Serial.println("Proxima sessao em 48 segundos.");
  Serial.println();
  sessaoCompleta = true;
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  atualizarLedEstado();
  randomSeed(analogRead(0));
  iniciarSessao();
}

void loop() {
  unsigned long agora = millis();

  if (indiceLeitura < NUM_LEITURAS && agora - ultimaLeituraMillis >= INTERVALO_LEITURA_MS) {
    ultimaLeituraMillis = agora;
    registrarLeitura();
    if (indiceLeitura == NUM_LEITURAS) {
      finalizarSessao();
    }
  }

  if (sessaoCompleta && agora - inicioSessaoMillis >= INTERVALO_SESSAO_MS) {
    iniciarSessao();
  }
}
