#include <NewPing.h>

#define M1_Izq 46 //Direccion High: avanzar  46
#define M1_Der 48 //Direccion High:Retroceder 48
#define M2_Izq 36 //Direccion High:retroceder 36
#define M2_Der 38 //Direccion High: avanzar 38
#define PWM_Der 3 //5
#define PWM_Izq 5 //3

#define Eco 50  //Sensor Izquierdo
#define Eco2 53
#define Disparo 51  //Sensor Izquierdo
#define Disparo2 52

NewPing sonar(51, 50, 300);
NewPing sonar2(52, 53, 300);

const float sonido=34300.0; //Velocidad del sonido en cm/s
int umbral=30;  
int flag1=0;  int estabilizado=0;
char estado='a';

unsigned long time;

  int dist,dist2;
  float distancia,distancia2;
  float phi/*Ángulo de ataque (rad)*/, pi=3.1415;
  float l,n,d=14.5/*Distancia entre sonares (cm)*/;
  float lptomedio, angulo;


void setup() {
  // put your setup code here, to run once:
  
  //Serial.begin(38400);    //No sirve 
  Serial1.begin(38400);   //El bluethooth escribe con un rango de bits de 38400, además, por lo comprobado hasta el momento solo funciona en Serial1
  
  
  pinMode(M1_Izq, OUTPUT);
  pinMode(M1_Der, OUTPUT);
  pinMode(M2_Izq, OUTPUT);
  pinMode(M2_Der, OUTPUT);
  pinMode(PWM_Der,OUTPUT);
  pinMode(PWM_Izq,OUTPUT);
  
  pinMode(Eco,INPUT);
  pinMode(Eco2,INPUT);
  pinMode(Disparo,OUTPUT);
  pinMode(Disparo2,OUTPUT);
  digitalWrite(Disparo, LOW);
  digitalWrite(Disparo2, LOW);
  stop();
}

void loop() {
  // put your main code here, to run repeatedly:        ¡¡¡¡IMPORTANTE!!! BUSCAR COMO EJECUTAR UNA TANGENTE EN LAS LIBRERÍAS

  switch (estado)
  {

  case 'a': //alinearse
    Serial1.println("Alineandose");
    distancia= calculardistancia()*10; distancia2= calculardistancia2()*10;
    dist= (int)distancia;    
    dist2= (int)distancia2; 
    if(dist>dist2) 
    {
      if(flag1=1){stop();flag1=0;} 
      girar(0, 100);
      Serial1.print("  -100  100 ");
    } //girar(0,v)
    else if (dist<dist2) 
    {
      if(flag1=0){stop();flag1=1;}
      girar(1, 100);
      Serial1.print("  100  -100 ");
      } //girar(1,v)
    else {estado='d'; stop();}
  break;

  case 'd': //dato
      if(dist2!=umbral){
      Serial1.println("Esperando dato");
      //umbral=Serial1.read();
      Serial1.print("Recibida la distancia ");
      Serial1.print((int)umbral);
      Serial1.println();
      }
      else delay (1000);   
        
      estado='p';
  break;

  case 'p': //posicionarse
  //if(dist2!=(umbral+1 || umbral-1))
  //{
    if(dist2>umbral) phi=pi/5;
    else phi=-pi/5;
    //n=tan(phi)*d*10; 
    angulo=atan((dist2-dist)/d*10)*10;
    Serial1.print(int(angulo));
    Serial1.print(" ");
    distancia= calculardistancia(); 
    dist= (int)distancia;   //Serial1.print(distancia); Serial1.print("  ");
    distancia2= calculardistancia2();   
    dist2= (int)distancia2;  //Serial1.print(distancia2);  Serial1.print(" ");
    if(int(angulo)<int(phi*10))/*dist>dist2-n)*/ //De esta manera se busca dist = dist2+n
    { 
      if(flag1=1){stop();flag1=0;} 
      girar(0, 110);
      Serial1.print("0  100 ");
    }
    else if (int(angulo)>int(phi*10))/*dist<dist2-n)*/ 
    { 
      if(flag1=0){stop();flag1=1;}
      girar(1, 110);
      Serial1.print("100  0 ");
    }    
    else {
        //lptomedio=dist2;//n/sqrt(1+tan(phi)*tan(phi))/*diferencia de distancia entre sonares perpendiculares a la pared*/*(1/2+dist/n);
        //Serial1.print(lptomedio);
        //Serial1.print(" ");
        if(dist2!=umbral) {avanzar(180); Serial1.print("-180  -180 ");} //avanzar
        //else if (dist2<umbral) {retroceder(180); Serial1.print("180  180 ");} //retroceder
        else {estado='a'; stop(); delay(1000); umbral=40;}
        } 
  //}
    break; 
  
  }
  time=millis();
  Serial1.print(time);
  Serial1.println();
  delay(50);
}


//Motor 1 rueda izquierda, relacionado con Disparo del sensor 1
//Motor 2 rueda derecha, relacionado con Disparo2 del sensor 2
void avanzar(int v){
    digitalWrite(M1_Izq, HIGH);
    digitalWrite(M1_Der, LOW);
    digitalWrite(M2_Izq, LOW);
    digitalWrite(M2_Der, HIGH); 
    analogWrite(PWM_Der,v);
    analogWrite(PWM_Izq,v);
  }

void retroceder(int v){
    digitalWrite(M1_Izq, LOW);
    digitalWrite(M1_Der, HIGH);
    digitalWrite(M2_Izq, HIGH);
    digitalWrite(M2_Der, LOW); 
    analogWrite(PWM_Der,v);
    analogWrite(PWM_Izq,v);
  }

void girar(int flag, int v){
  if (flag==0) {    //Giro de sentido horario
    digitalWrite(M1_Izq, HIGH);
    digitalWrite(M1_Der, LOW);
    digitalWrite(M2_Izq, LOW);// HIGH);
    digitalWrite(M2_Der, LOW); 
    analogWrite(PWM_Der, v);
    analogWrite(PWM_Izq, v);
    }
  else{    //Giro de sentido antihorario
    digitalWrite(M1_Izq, LOW);
    digitalWrite(M1_Der, LOW);// HIGH);
    digitalWrite(M2_Izq, LOW);
    digitalWrite(M2_Der, HIGH); 
    analogWrite(PWM_Der, v);
    analogWrite(PWM_Izq, v);
    } 
  }  

void stop(){
    digitalWrite(M1_Izq, LOW);
    digitalWrite(M1_Der, LOW);
    digitalWrite(M2_Izq, LOW);
    digitalWrite(M2_Der, LOW);
    analogWrite(PWM_Der,0);
    analogWrite(PWM_Izq,0);
  }

float calculardistancia(){
  unsigned long tiempo=sonar.ping(); 
  float distancia=tiempo*0.000001*sonido/4.0;
  Serial1.print(int(distancia));
  Serial1.print(" ");
  //Serial1.println();
  
  return distancia;
}

float calculardistancia2(){
  unsigned long tiempo=sonar2.ping(); 
  float distancia=tiempo*0.000001*sonido/4.0;
  Serial1.print(int(distancia));
  Serial1.print(" ");
  //Serial1.println();
  
  return distancia;
}

