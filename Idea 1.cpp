#include <iostream>
#include <string>
#include <ctime>
#include <fstream>

using namespace std;

const int FILAS = 16;
const int COLUMNAS = 16;
const string CLAVE = "1234";

char mapa[FILAS][COLUMNAS];
string placas[FILAS][COLUMNAS];
int tipoVehiculo[FILAS][COLUMNAS];
int tipoUsuario[FILAS][COLUMNAS];
time_t horaEntrada[FILAS][COLUMNAS];
bool cargando[FILAS][COLUMNAS];

// Inicializar
void inicializarMapa() {
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            mapa[i][j] = '.';

            if (i < 4 && j < 4) mapa[i][j] = 'B';
            else if (i < 4 && j < 8) mapa[i][j] = 'E';
            else if (i > 5 && j > 5) mapa[i][j] = 'P';
        }
    }
    mapa[0][0] = 'I';
    mapa[FILAS-1][COLUMNAS-1] = 'S';
}

// Mostrar mapa
void mostrarMapa() {
    cout << "\nMAPA\n";
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            cout << mapa[i][j] << " ";
        }
        cout << endl;
    }
}

// Buscar espacio más cercano
bool asignarEspacio(int tipo, bool carga, int &x, int &y) {
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {

            if (tipo == 3 && mapa[i][j] == 'B') { x=i; y=j; return true; }
            if (tipo == 4 && mapa[i][j] == 'E') { x=i; y=j; return true; }
            if ((tipo == 1 || tipo == 2) && mapa[i][j] == 'P') { x=i; y=j; return true; }

        }
    }
    return false;
}

// Calcular pago
int calcularPago(int tipo, int tiempo, bool carga, int usuario, int hora) {
    int costo = 0;

    if (tipo == 1) costo = 2000 * tiempo;
    if (tipo == 2) costo = 1000 * tiempo;
    if (tipo == 3) costo = 500 * tiempo;
    if (tipo == 4) costo = 2500 * tiempo;

    if (carga) costo += 500 * tiempo;

    // VIP descuento
    if (usuario == 2) costo *= 0.8;

    // Empleado gratis
    if (usuario == 3) costo = 0;

    // Nocturno
    if (hora >= 18) costo *= 0.8;

    // Multa
    if (tiempo > 480) costo += 10000;

    return costo;
}

// Ingresar
void ingresarVehiculo() {
    string placa;
    int tipo, usuario;
    bool carga = false;

    cout << "Placa: ";
    cin >> placa;

    cout << "Tipo (1=Carro,2=Moto,3=Bici,4=Electrico): ";
    cin >> tipo;

    cout << "Usuario (1=Normal,2=VIP,3=Empleado): ";
    cin >> usuario;

    if (tipo == 4) {
        string respuesta;

do {
    cout << "¿Desea cargar el vehiculo? (si/no): ";
    cin >> respuesta;

    if (respuesta == "si" || respuesta == "SI") {
        carga = true;
        break;
    } 
    else if (respuesta == "no" || respuesta == "NO") {
        carga = false;
        break;
    } 
    else {
        cout << "Respuesta invalida\n";
    }

} while (true);
    }

    int x,y;
    if (!asignarEspacio(tipo,carga,x,y)) {
        cout << "🚨 Parqueadero lleno\n";
        return;
    }

    mapa[x][y] = (tipo==4 && carga)?'C':'X';
    placas[x][y] = placa;
    tipoVehiculo[x][y] = tipo;
    tipoUsuario[x][y] = usuario;
    horaEntrada[x][y] = time(0);
    cargando[x][y] = carga;

    cout << "\n----- TICKET -----\n";
    cout << "Placa: " << placa << endl;
    cout << "Posicion: " << x << "," << y << endl;
    cout << "------------------\n";
}

// Retirar
void retirarVehiculo() {
    string placa;
    cout << "Placa: ";
    cin >> placa;

    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {

            if (placas[i][j] == placa) {

                time_t salida = time(0);
                int tiempo = (salida - horaEntrada[i][j]) / 60;
                if (tiempo == 0) tiempo = 1;

                tm *ltm = localtime(&salida);
                int hora = ltm->tm_hour;

                int pago = calcularPago(tipoVehiculo[i][j], tiempo, cargando[i][j], tipoUsuario[i][j], hora);

                cout << "💰 Total: " << pago << endl;

                ofstream archivo("registro.txt", ios::app);
                archivo << placa << " " << pago << endl;
                archivo.close();

                if (tipoVehiculo[i][j] == 3) mapa[i][j] = 'B';
                else if (tipoVehiculo[i][j] == 4) mapa[i][j] = 'E';
                else mapa[i][j] = 'P';

                placas[i][j] = "";

                return;
            }
        }
    }

    cout << "No encontrado\n";
}

// Buscar
void buscarVehiculo() {
    string placa;
    cin >> placa;

    for (int i = 0; i < FILAS; i++)
        for (int j = 0; j < COLUMNAS; j++)
            if (placas[i][j] == placa)
                cout << "Pos: " << i << "," << j << endl;
}

// Reservar (con clave)
void reservar() {
    string clave;
    cout << "Clave: ";
    cin >> clave;

    if (clave != CLAVE) {
        cout << "Acceso denegado\n";
        return;
    }

    for (int i=0;i<FILAS;i++)
        for (int j=0;j<COLUMNAS;j++)
            if (mapa[i][j]=='P') {
                mapa[i][j]='R';
                cout<<"Reservado\n";
                return;
            }
}

// Estadísticas (con clave)
void estadisticas() {
    string clave;
    cout << "Clave: ";
    cin >> clave;

    if (clave != CLAVE) {
        cout << "Acceso denegado\n";
        return;
    }

    ifstream archivo("registro.txt");
    string linea;
    int total=0;

    while(getline(archivo,linea)) total++;

    cout << "Vehiculos atendidos: " << total << endl;
}

// Disponibilidad
void disponibilidad() {
    int libres=0,ocupados=0;

    for (int i=0;i<FILAS;i++)
        for (int j=0;j<COLUMNAS;j++)
            if (mapa[i][j]=='X'||mapa[i][j]=='C') ocupados++;
            else if (mapa[i][j]=='P'||mapa[i][j]=='B'||mapa[i][j]=='E') libres++;

    cout << "Libres: " << libres << " Ocupados: " << ocupados << endl;
}

// MAIN
int main() {
    inicializarMapa();
    int op;

    do {
        cout << "\n1.Mapa\n2.Ingresar\n3.Retirar\n4.Disponibilidad\n5.Buscar\n6.Reservar\n7.Estadisticas\n8.Salir\n";
        cin >> op;

        switch(op) {
            case 1: mostrarMapa(); break;
            case 2: ingresarVehiculo(); break;
            case 3: retirarVehiculo(); break;
            case 4: disponibilidad(); break;
            case 5: buscarVehiculo(); break;
            case 6: reservar(); break;
            case 7: estadisticas(); break;
        }

    } while(op!=8);

    return 0;
}
