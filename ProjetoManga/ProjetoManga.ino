#include "string.h"
#include <LiquidCrystal_I2C.h>

const int SENSOR_MANGA_RUIM = 12;
const int SENSOR_ESTEIRA = 11;
const int ATUADOR_ESTEIRA = 7;
const int BOTAO_ESTEIRA = 8;

const int MOTOR_VELOCIDADE = 6;
const int MOTOR_ANTI_HORARIO = 9;
const int MOTOR = 10;



bool ESTADO_ULTIMO_BOTAO_ESTEIRA = LOW;
bool ESTADO_ATUAL_BOTAO_ESTEIRA = LOW;

bool MOTOR_LIGADO = false;
int VELOCIDADE_MOTOR = 0;
int SENSOR_MANGA_RUIM_LIGADO = 0;

int QUANTIDADE_MANGA_RUIM = 0;
int QUANTIDADE_MANGA_BOA = 0;

LiquidCrystal_I2C lcd(0x27,16,2);


void setup() {
  pinMode(ATUADOR_ESTEIRA, OUTPUT);
  pinMode(MOTOR, OUTPUT);
  pinMode(MOTOR_ANTI_HORARIO, OUTPUT);
  pinMode(MOTOR_VELOCIDADE, OUTPUT);
  pinMode(BOTAO_ESTEIRA, INPUT);
  pinMode(SENSOR_ESTEIRA, INPUT);
  pinMode(SENSOR_MANGA_RUIM, INPUT);

  digitalWrite(ATUADOR_ESTEIRA, HIGH);
  digitalWrite(MOTOR_ANTI_HORARIO, LOW);
  digitalWrite(MOTOR, HIGH);

  Serial.begin(115200); // Initialize the software serial port

  lcd.init(); // initialize the lcd
  lcd.setBacklight(HIGH);
  lcd.setCursor(0,0);
  lcd.print("Manga Boa : " + String(QUANTIDADE_MANGA_BOA) + " ");
  lcd.setCursor(0,1);  
  lcd.print("Manga Ruim: " + String(QUANTIDADE_MANGA_RUIM) + " ");
}

void loop() {

  defineEstadoMotor();
  defineDestinoManga();

}

void defineDestinoManga() {

  if (digitalRead(SENSOR_ESTEIRA) == 0 && MOTOR_LIGADO == 1) {

    defineEstadoMotor(false, 5);

    if (digitalRead(SENSOR_ESTEIRA) == 0)
    {

      //Serial.println("Motor depois do if: " + String(MOTOR_LIGADO));

      Serial.write("analisar"); 

      int contadorRetornoEsp32 = 0;
      bool mangaBoa = false;
      String retornoEstadoManga;

      while (contadorRetornoEsp32 <= 8) {

        delay(25);
        retornoEstadoManga = Serial.readString();

        Serial.println("Retorno estado: " + retornoEstadoManga);
        Serial.println("Aguardando...");

        if (String(retornoEstadoManga) == "boa" || String(retornoEstadoManga) == "ruim" || String(retornoEstadoManga) == "parar") {
          break;
          delay(100);
        }

        contadorRetornoEsp32++;
      }

      if (String(retornoEstadoManga) == "ruim") {

        defineQuantidadeMangaLcd("ruim");

        defineEstadoMotor(true, 700);

        while (digitalRead(SENSOR_ESTEIRA) == 0) {
            delay(5);
        }
      }

      else if (String(retornoEstadoManga) == "boa"){
       
        defineQuantidadeMangaLcd("boa");

        digitalWrite(ATUADOR_ESTEIRA, LOW);

        delay(200);

        digitalWrite(ATUADOR_ESTEIRA, HIGH);

        defineEstadoMotor(true, 500);
      }

      else if (String(retornoEstadoManga) == "parar") {
        defineEstadoMotor(false, 50);
      }

      else {

        defineEstadoMotor(true, 50);
        while (digitalRead(SENSOR_ESTEIRA) == 0) {
            delay(5);
        }
        
      }
    }
  } 
}

void defineQuantidadeMangaLcd(String estadoManga) {

  if (estadoManga.equals("boa")) {
    QUANTIDADE_MANGA_BOA++; 
    delay(50);
    lcd.setCursor(0,0);
    lcd.print("Manga boa: " + String(QUANTIDADE_MANGA_BOA) + " ");
  }

  else {
    QUANTIDADE_MANGA_RUIM++; 
    delay(50);
    lcd.setCursor(0,1);
    lcd.print("Manga ruim: " + String(QUANTIDADE_MANGA_RUIM) + " ");
  }
}

void defineEstadoMotor(bool estadoMotor, int tempo) {

  delay(50);

  MOTOR_LIGADO = estadoMotor;

  digitalWrite(MOTOR, MOTOR_LIGADO);

  delay(tempo);
}

void defineEstadoMotor() {

  ESTADO_ATUAL_BOTAO_ESTEIRA = estadoBotao(ESTADO_ULTIMO_BOTAO_ESTEIRA);

  if (ESTADO_ULTIMO_BOTAO_ESTEIRA == LOW && ESTADO_ATUAL_BOTAO_ESTEIRA == HIGH)
  {
      MOTOR_LIGADO = !MOTOR_LIGADO;
      delay(100);
  }

  ESTADO_ULTIMO_BOTAO_ESTEIRA = ESTADO_ATUAL_BOTAO_ESTEIRA;
  
  digitalWrite(MOTOR, MOTOR_LIGADO);

  if (MOTOR_LIGADO) {
    int VELOCIDADE_ATUAL_MOTOR = analogRead(A0);
    VELOCIDADE_ATUAL_MOTOR = VELOCIDADE_ATUAL_MOTOR * 0.2492668622;

    if (VELOCIDADE_ATUAL_MOTOR <= 255) {
      analogWrite(MOTOR_VELOCIDADE, VELOCIDADE_ATUAL_MOTOR);
    }
  }
}

bool estadoBotao(bool estadoUltimoBotao) {

  bool estadoAtualBotao = digitalRead(BOTAO_ESTEIRA);

  if (estadoAtualBotao != estadoUltimoBotao)
  {
    delay(5);
    estadoAtualBotao = digitalRead(BOTAO_ESTEIRA);
  }

  return estadoAtualBotao;
}





