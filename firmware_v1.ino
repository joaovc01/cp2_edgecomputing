#define LED_R 25
#define LED_G 26
#define LED_B 27
 
const uint8_t NUM_LEITURAS = 5;
const unsigned long INTERVALO_LEITURA_MS = 2000;
const unsigned long INTERVALO_SESSAO_MS = 48000;
 
int leituras[NUM_LEITURAS];
uint8_t indiceLeitura = 0;
unsigned long inicioSessaoMillis = 0;
unsigned long ultimaLeituraMillis = 0;
bool sessaoCompleta = false;
 
void definirLed(bool r, bool g, bool b) {
  digitalWrite(LED_R, r ? HIGH : LOW);
  digitalWrite(LED_G, g ? HIGH : LOW);
  digitalWrite(LED_B, b ? HIGH : LOW);
}
 
int gerarLeitura() {
  return random(10, 21);
}
 
void iniciarSessao() {
  Serial.println("========================================");
  Serial.println("MONITORAMENTO DE VEGETACAO - FW 1.0");
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
 
void finalizarSessao() {
  float soma = 0;
  for (uint8_t i = 0; i < NUM_LEITURAS; i++) {
    soma += leituras[i];
  }
  float media = soma / NUM_LEITURAS;
  Serial.print("Media da sessao: ");
  Serial.print(media, 1);
  Serial.println(" cm");
  Serial.println("Proxima sessao em 48 segundos.");
  Serial.println();
  sessaoCompleta = true;
}
 
void setup() {
  Serial.begin(115200);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  definirLed(false, false, true);
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
 
