#include <AFMotor.h>
#include <Servo.h>
#include <NewPing.h>
 
#define TRIG_PIN A3 // Conecta TRIG do sensor ultrassom à porta analógica A0
#define ECHO_PIN A2 // Conecta ECHO do sensor ultrassom à porta analógica A5
#define TRIG_PIN_FD A1 // Conecta TRIG do sensor ultrassom à porta analógica A0
#define ECHO_PIN_FD A0 // Conecta ECHO do sensor ultrassom à porta analógica A5
#define MICROPHONE_PIN_DIG 2 //Pino digital ligado ao sensor de som (DO)
#define SERVO_PIN 10
#define MAX_DISTANCE 300         // Distância máxima de medição do sensor ultrassom. Retorna zero se passar desse valor.
#define MAX_SPEED 150            // Velocidade dos motores de tração.
#define MAX_SPEED_OFFSET 40      // Offset de velocidade utilizado para fazer curvas.
#define COLL_DIST 30            // Distância de colisão horizontal
#define FALL_DOWN_DIST 75       // Distancia de queda
 
AF_DCMotor leftMotorI(1, MOTOR12_1KHZ);
AF_DCMotor leftMotorII(2, MOTOR12_1KHZ);
AF_DCMotor rightMotorI(3, MOTOR34_1KHZ);
AF_DCMotor rightMotorII(4, MOTOR34_1KHZ);
 
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);
NewPing sonarFallDown(TRIG_PIN_FD, ECHO_PIN_FD, MAX_DISTANCE);

#define bool int
#define true 1
#define false 0
 
Servo servoMotor;

int speedSet = 0;

int clapSize = 0; // Contador de Palmas
int clapsToChangeState = 2; // Contagem pra parar/ligar o carro
bool isRunning = false;
bool movedForward = false;

// Tempo máximo entre o pulso seguinte
unsigned long maxSoundTime = 1000;
unsigned long minSoundTime = 300;
unsigned long SoundLength = 100; // Comprimento do som esperado
unsigned long time;
unsigned long startTime = 0;
 
void setupServo(Servo servo)
{
    //Conecta o servo na porta 10.
    servo.attach(SERVO_PIN);
    //Aponta para frente.
    servo.write(90);
    delay(1000);
}

void setupSoundSensor()
{
    //pinMode(MICROPHONE_PIN_ANALOG, INPUT);
    pinMode(MICROPHONE_PIN_DIG, INPUT);
}
 
void setup()
{
    Serial.begin(9600);
    //Inicializa motor servo.
    setupServo(servoMotor);
    setupSoundSensor();
}
 
void loop()
{
    //No loop principal, aguardamos duas palmas para iniciar o carro.
    //Após isso, lemos a distancia medida pelo sensor ultrassom constantemente,
    //e caso uma obstrução seja detectada em seu caminho, procura uma melhor alternativa
    //de trajeto.
    if(isRunning==true)
    {
        initialMovement();
        int curDist = readPing(sonar);
        int fallDownDist = readPing(sonarFallDown);
        //Serial.print("curDist");
        //Serial.println(curDist);
        //Serial.print("fallDownDist");
        //Serial.println(fallDownDist);
        if (curDist < COLL_DIST && curDist != 0) changePath();
        if (fallDownDist > FALL_DOWN_DIST && fallDownDist != 0) changePath(); 
    }
    else
    {
        verifyClap();
    }
}
 
long unsigned int moveServoLeft()
{
    int moveTime = 500;
    //Direciona o sensor à esquerda.
    servoMotor.write(36);
    return moveTime;
}
 
long unsigned int moveServoRight()
{
    int moveTime = 600;
    //Direciona o sensor à direita.
    servoMotor.write(144);
    return moveTime;
}
 
//Mede as distâncias à esquerda, e à direita,
//escolhendo o melhor caminho para evitar colisões.
void changePath()
{
    //Para os motores.
    moveStop();
    delay(moveServoLeft());
    //Mede a distância à direita.
    int rightDistance = readPing(sonar);
    //Serial.print("Right: ");
    //Serial.println(rightDistance);
    delay(moveServoRight());
    //Mede a distância à esquerda.
    int leftDistance = readPing(sonar);
    //Serial.print("Left: ");
    //Serial.println(leftDistance);
    //Retorna sensor ao centro.
    servoMotor.write(90);
    //Manobra para evitar colisão.
    chooseBestMovement(leftDistance, rightDistance);
}
 
//Determina o melhor plano de ação, dadas as duas distâncias medidas.
void chooseBestMovement(int leftDistance, int rightDistance)
{
    //Se a distancia medida for zero, passou dos limites de medição do sensor,
    //logo, é uma boa opção para evitar colisão.
    int moveTime;
    if (leftDistance > rightDistance) moveTime = turnLeft();
    else if (rightDistance > leftDistance) moveTime = turnRight();
    else moveTime = turnAround();
    //Delay passando o tempo da manobra, para evitar falsas colisões.
    delay(moveTime);
    //Move para frente novamente.
    moveForward();
}
 
//Lê 5 distâncias medidas pelo sensor e retorna a média.
int readPing(NewPing sn)
{
    unsigned int uS = 0;
    unsigned int numMed = 5;
    for(int i = 0; i < numMed; i++)
    {
        unsigned int med = sn.ping();
        uS += med;
        delay(10);
    }
    return uS / (numMed*US_ROUNDTRIP_CM);
}
 
//Para todos os motores.
void moveStop()
{
    leftMotorI.run(RELEASE);
    leftMotorII.run(RELEASE);
    rightMotorI.run(RELEASE);
    rightMotorII.run(RELEASE);
}
 
//Movimento para frente
void moveForward()
{
    leftMotorI.run(FORWARD);                                // Move para frente.
    leftMotorII.run(FORWARD);                               // Move para frente.
    rightMotorI.run(FORWARD);                               // Move para frente.
    rightMotorII.run(FORWARD);                              // Move para frente.
    speedUp();
}
 
//Movimento para trás.
void moveBackward()
{
    leftMotorI.run(BACKWARD);                                // Muda para marcha ré.
    leftMotorII.run(BACKWARD);                               // Muda para marcha ré.
    rightMotorI.run(BACKWARD);                               // Muda para marcha ré.
    rightMotorII.run(BACKWARD);                              // Muda para marcha ré.
    speedUp();
}
 
//Aumenta velocidade gradativamente para evitar perda de bateria.
void speedUp()
{
    for (speedSet = 0; speedSet < MAX_SPEED; speedSet += 2)
    {
      leftMotorI.setSpeed(speedSet);
      leftMotorII.setSpeed(speedSet);
      rightMotorI.setSpeed(speedSet);
      rightMotorII.setSpeed(speedSet);
      delay(5);
    }
}
 
//Vira à direita.
int turnRight()
{
    //Serial.println("Turned right!");
    int moveTime = 350;
    leftMotorI.run(FORWARD);   // turn motor 1 forward
    leftMotorII.run(FORWARD);  // turn motor 2 forward
    rightMotorI.run(BACKWARD);  // turn motor 3 FORWARD
    rightMotorII.run(BACKWARD); // turn motor 4 FORWARD
    rightMotorI.setSpeed(speedSet + MAX_SPEED_OFFSET);
    rightMotorII.setSpeed(speedSet + MAX_SPEED_OFFSET);
    return moveTime;
}
 
//Vira à esquerda.
int turnLeft()
{
    //Serial.println("Turned left!");
    int moveTime = 350;
    leftMotorI.run(BACKWARD);  // turn motor 1 FORWARD
    leftMotorII.run(BACKWARD); // turn motor 2 FORWARD
    leftMotorI.setSpeed(speedSet + MAX_SPEED_OFFSET);
    leftMotorII.setSpeed(speedSet + MAX_SPEED_OFFSET);
    rightMotorI.run(FORWARD);  // turn motor 3 forward
    rightMotorII.run(FORWARD); // turn motor 4 forward
    return moveTime;
}
 
//Vira 180 graus.
int turnAround()
{
    //Serial.println("Turned around!");
    int moveTime = 500;
    leftMotorI.run(FORWARD);   // turn motor 1 forward
    leftMotorII.run(FORWARD);  // turn motor 2 forward
    rightMotorI.run(FORWARD);  // turn motor 3 FORWARD
    rightMotorII.run(FORWARD); // turn motor 4 FORWARD
    rightMotorI.setSpeed(speedSet + MAX_SPEED_OFFSET);
    rightMotorII.setSpeed(speedSet + MAX_SPEED_OFFSET);
    return moveTime;
}

void initialMovement()
{
    if(movedForward == false)
    {
        moveForward();
        movedForward = true;
    }
}
 
void verifyClap()
{
    time = millis(); // Inicia a contagem de tempo
    unsigned long timeAfterClap = time - startTime; // Verifica o tempo entre o pulso inicial e o seguinte
   
    if (timeAfterClap >= SoundLength && digitalRead(MICROPHONE_PIN_DIG) == HIGH)
    {
        // Verifica se o pulso recebido respeita o intervalo entre 1 pulso e outro
        if (timeAfterClap < minSoundTime || timeAfterClap > maxSoundTime)
        {
            // Caso contrario o intervalo resetara a contagem e o tempo
            clapSize = 0;
            startTime = millis();
            //Serial.println("resetaContagem");
        }
        else
        {
            // Iniciada a contagem de pulso e o tempo
            clapSize ++;
            startTime = millis();
            //Serial.println("ClapSize++");
        }
        // Verifica se a contagem de palma é igual ou superior ao valor para mudar o estado
        if(clapSize>=clapsToChangeState-1)
        {
            if(isRunning==true)
            {
                //Serial.println("isRunning=false");
                isRunning = false;
            }
            else
            {
                //Serial.println("isRunning=true");
                isRunning = true;
            }
            clapSize = 0;
        }
    }
}
