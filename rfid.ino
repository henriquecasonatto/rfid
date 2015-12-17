//
// Controle de Acesso RFID
//(C)2013 Henrique Casonatto
//
#define saidaExterna 12
#define ledPin 13
#define desbloqueioTempo 2
//
//Banco de Dados ///
//
//banco de rfid (codigo)
char* tagsPermitidas[] = {
"[Inserir identificadores aqui]", // Tag 1
"12345678E95", // Tag 2
"1123ABC4567", // Tag 3
};
//banco de nome de tags
char* nomeTag[] = {
"[ADD YOUR NAME HERE]", // Tag 1
"Henrique", // Tag 2
"NAME 3", // Tag 3
};
int numerodeTags = sizeof(tagsPermitidas)/sizeof(tagsPermitidas[0]);
int incomingByte = 0; // Para armazenar a porta serial
//
//:::SETUP:::
//
void setup() {
pinMode(ledPin, OUTPUT);
digitalWrite(ledPin, LOW);
pinMode(saidaExterna, OUTPUT);
digitalWrite(saidaExterna, LOW);

Serial.begin(9600); 
Serial.println("(C)2013 Henrique Casonatto REV0.02");
Serial.println("Sistema Rfid Inicializado.");
}
//
//:::LOOP:::
//
void loop() {
byte i = 0;
byte val = 0;
byte verificador = 0;
byte bytesLidos = 0;
byte byteTemporario = 0;
byte tagBytes[6]; //as tags usam apenas 5 bytes mas presisamos de um byte a mais pro verificador
char tagemRaw[10];
if((val = Serial.read()) == 2) {
bytesLidos = 0;
while (bytesLidos < 12) { // le os 10 digitos do codigo mais 2 do verificador
val = Serial.read(); 

if (bytesLidos < 10)
{
tagemRaw[bytesLidos] = val;
}

//gambiarras a parte
if((val == 0x0D)||(val == 0x0A)||(val == 0x03)||(val == 0x02)) {
break; 
}

// conversao Ascii/Hex 
if ((val >= '0') && (val <= '9')) {
val = val - '0';
}
else if ((val >= 'A') && (val <= 'F')) {
val = 10 + val - 'A';
}

//mais gambi pros bytes 
if (bytesLidos & 1 == 1) {
tagBytes[bytesLidos >> 1] = (val | (byteTemporario << 4));
if (bytesLidos >> 1 != 5) { 
verificador ^= tagBytes[bytesLidos >> 1]; //aki testa se a tag e verdadeira com um XOR
};
} else {
byteTemporario = val;
};

bytesLidos++; 
}

//
// envia o resultado via serial
//
if (bytesLidos == 12) { //verifica se termino a leitura
tagemRaw[10] = '\0'; // pula a linha hehe

Serial.print("Tag Identificada: ");
for (i=0; i<5; i++) {
if (tagBytes[i] < 16) {
Serial.print("0");
}
Serial.print(tagBytes[i], HEX);
}
Serial.println();
Serial.print("Teste: ");
Serial.print(tagBytes[5], HEX);
Serial.println(tagBytes[5] == verificador ? " -- passou." : " -- erro.");
Serial.print("RAW: ");
Serial.println(tagemRaw);

// encontra a tag no banco de dados
int identificacaotag = encontreTag( tagemRaw );
//se encontrou no banco de dados :
if( identificacaotag > 0 )
{
Serial.print("Numero da tag autorizada ");
Serial.print(identificacaotag);
Serial.print(": Desbloqueando Para  ");
Serial.println(nomeTag[identificacaotag - 1]);
desbloqueio(); 
} else {
Serial.println("Tag nao autorizada");
}
Serial.println(); //separador em branco
}

bytesLidos = 0;
}
}

/**
* funcao de Desbloqueio
*/
void desbloqueio() {
digitalWrite(ledPin, HIGH);
digitalWrite(saidaExterna, HIGH);
delay(desbloqueioTempo * 1000);
digitalWrite(saidaExterna, LOW);
digitalWrite(ledPin, LOW);
}

/**
* funcao de busca de tags
retorna 0 se nao encontra 
retorna 1 se encontra
*/
int encontreTag( char tagemRaw[10] ) {
for (int estatag = 0; estatag < numerodeTags; estatag++) {
if(strcmp(tagemRaw, tagsPermitidas[estatag]) == 0)
{
return(estatag + 1);
}
}
return(0);
} 
