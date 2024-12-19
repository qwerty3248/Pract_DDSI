#include <iostream>
#include <sql.h>
#include <sqlext.h>
#include <string>

void conectar_bd(SQLHDBC &dbc) {
    SQLHENV env;
    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
    SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
    SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);
    
    // Cadena de conexión
    std::string connStr = "DRIVER={Oracle ODBC Driver};DBQ=practbd;UID=x7201609;PWD=x7201609;";
    
    SQLCHAR retConnStr[1024];
    SQLSMALLINT retConnStrLen;

    // Conectar a la base de datos
    SQLRETURN ret = SQLDriverConnect(dbc, NULL, (SQLCHAR*)connStr.c_str(), SQL_NTS,
                                     retConnStr, sizeof(retConnStr), &retConnStrLen,
                                     SQL_DRIVER_COMPLETE);
    
    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        std::cout << "Conexión establecida con éxito." << std::endl;
    } else {
        std::cerr << "Error al conectar a la base de datos." << std::endl;
    }
}

void crear_tablas(SQLHDBC dbc) {
    SQLHSTMT stmt;
    SQLRETURN ret;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    // Eliminar tablas existentes
    try {
        SQLExecDirect(stmt, (SQLCHAR*)"DROP TABLE Detalle_Pedido", SQL_NTS);
        SQLExecDirect(stmt, (SQLCHAR*)"DROP TABLE Pedido", SQL_NTS);
        SQLExecDirect(stmt, (SQLCHAR*)"DROP TABLE Stock", SQL_NTS);
    } catch (...) {
        // Ignorar errores si las tablas no existen
    }

    // Crear la tabla Stock
    SQLExecDirect(stmt, (SQLCHAR*)"CREATE TABLE Stock (Cproducto INT PRIMARY KEY, Cantidad INT)", SQL_NTS);

    // Crear la tabla Pedido
    SQLExecDirect(stmt, (SQLCHAR*)"CREATE TABLE Pedido (Cpedido INT PRIMARY KEY, Ccliente INT, Fecha_pedido DATE)", SQL_NTS);

    // Crear la tabla Detalle_Pedido
    SQLExecDirect(stmt, (SQLCHAR*)"CREATE TABLE Detalle_Pedido (Cpedido INT, Cproducto INT, Cantidad INT, PRIMARY KEY (Cpedido, Cproducto), FOREIGN KEY (Cpedido) REFERENCES Pedido(Cpedido), FOREIGN KEY (Cproducto) REFERENCES Stock(Cproducto))", SQL_NTS);

    // Insertar 10 tuplas predefinidas en la tabla Stock
    SQLINTEGER productos[10][2] = {
        {1, 100}, {2, 150}, {3, 200}, {4, 250}, {5, 300},
        {6, 350}, {7, 400}, {8, 450}, {9, 500}, {10, 550}
    };

    for (int i = 0; i < 10; i++) {
        SQLPrepare(stmt, (SQLCHAR*)"INSERT INTO Stock (Cproducto, Cantidad) VALUES (?, ?)", SQL_NTS);
        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &productos[i][0], 0, NULL);
        SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &productos[i][1], 0, NULL);
        SQLExecute(stmt);
    }

    std::cout << "Tablas creadas y 10 tuplas insertadas en Stock." << std::endl;

    // Liberar recursos
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}

void mostrar_tablas(SQLHDBC dbc) {
    SQLHSTMT stmt;
    SQLRETURN ret;

    // Crear la declaración
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    // Mostrar Stock
    std::cout << "\nTabla Stock:" << std::endl;
    ret = SQLExecDirect(stmt, (SQLCHAR*)"SELECT * FROM Stock", SQL_NTS);
    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        SQLINTEGER cproducto, cantidad;
        while (SQLFetch(stmt) == SQL_SUCCESS) {
            SQLGetData(stmt, 1, SQL_C_LONG, &cproducto, 0, NULL);
            SQLGetData(stmt, 2, SQL_C_LONG, &cantidad, 0, NULL);
            std::cout << "Cproducto: " << cproducto << ", Cantidad: " << cantidad << std::endl;
        }
    }

    // Mostrar Pedido
    std::cout << "\nTabla Pedido:" << std::endl;
    ret = SQLExecDirect(stmt, (SQLCHAR*)"SELECT * FROM Pedido", SQL_NTS);
    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        SQLINTEGER cpedido, ccliente;
        SQL_DATE_STRUCT fecha_pedido;
        while (SQLFetch(stmt) == SQL_SUCCESS) {
            SQLGetData(stmt, 1, SQL_C_LONG, &cpedido, 0, NULL);
            SQLGetData(stmt, 2, SQL_C_LONG, &ccliente, 0, NULL);
            SQLGetData(stmt, 3, SQL_C_TYPE_DATE, &fecha_pedido, sizeof(fecha_pedido), NULL);
            std::cout << "Cpedido: " << cpedido << ", Ccliente: " << ccliente 
                      << ", Fecha_pedido: " << fecha_pedido.year << "-" << fecha_pedido.month << "-" << fecha_pedido.day << std::endl;
        }
    }

    // Mostrar Detalle_Pedido
    std::cout << "\nTabla Detalle_Pedido:" << std::endl;
    ret = SQLExecDirect(stmt, (SQLCHAR*)"SELECT * FROM Detalle_Pedido", SQL_NTS);
    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        SQLINTEGER cpedido, cproducto, cantidad;
        while (SQLFetch(stmt) == SQL_SUCCESS) {
            SQLGetData(stmt, 1, SQL_C_LONG, &cpedido, 0, NULL);
            SQLGetData(stmt, 2, SQL_C_LONG, &cproducto, 0, NULL);
            SQLGetData(stmt, 3, SQL_C_LONG, &cantidad, 0, NULL);
            std::cout << "Cpedido: " << cpedido << ", Cproducto: " << cproducto 
                      << ", Cantidad: " << cantidad << std::endl;
        }
    }

    // Liberar recursos
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}

void dar_alta_nuevo_pedido(SQLHDBC dbc) {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    // Obtener datos del nuevo pedido
    SQLINTEGER cpedido, ccliente;
    std::cout << "Introduce el código del pedido: ";
    std::cin >> cpedido;
    std::cout << "Introduce el código del cliente: ";
    std::cin >> ccliente;

    // Insertar el pedido en la tabla Pedido
    SQLPrepare(stmt, (SQLCHAR*)"INSERT INTO Pedido (Cpedido, Ccliente, Fecha_pedido) VALUES (?, ?, SYSDATE)", SQL_NTS);
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &cpedido, 0, NULL);
    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &ccliente, 0, NULL);
    SQLExecute(stmt);

    while (true) {
        std::cout << "\n1. Añadir detalle de producto" << std::endl;
        std::cout << "2. Eliminar todos los detalles de producto" << std::endl;
        std::cout << "3. Cancelar pedido" << std::endl;
        std::cout << "4. Finalizar pedido" << std::endl;

        int opcion;
        std::cout << "\nSelecciona una opción: ";
        std::cin >> opcion;

        if (opcion == 1) {
            // Añadir detalle del producto
            SQLINTEGER cproducto, cantidad;
            std::cout << "Introduce el código del producto: ";
            std::cin >> cproducto;
            std::cout << "Introduce la cantidad: ";
            std::cin >> cantidad;

            // Verificar si hay stock suficiente
            SQLPrepare(stmt, (SQLCHAR*)"SELECT Cantidad FROM Stock WHERE Cproducto = ?", SQL_NTS);
            SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &cproducto, 0, NULL);
            SQLExecute(stmt);
            SQLINTEGER stockCantidad;
            SQLFetch(stmt);
            SQLGetData(stmt, 1, SQL_C_LONG, &stockCantidad, 0, NULL);

            if (stockCantidad >= cantidad) {
                // Insertar en Detalle_Pedido y actualizar Stock
                SQLPrepare(stmt, (SQLCHAR*)"INSERT INTO Detalle_Pedido (Cpedido, Cproducto, Cantidad) VALUES (?, ?, ?)", SQL_NTS);
                SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &cpedido, 0, NULL);
                SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &cproducto, 0, NULL);
                SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &cantidad, 0, NULL);
                SQLExecute(stmt);

                // Actualizar stock
                SQLPrepare(stmt, (SQLCHAR*)"UPDATE Stock SET Cantidad = Cantidad - ? WHERE Cproducto = ?", SQL_NTS);
                SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &cantidad, 0, NULL);
                SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &cproducto, 0, NULL);
                SQLExecute(stmt);
                std::cout << "Detalle añadido con éxito." << std::endl;
            } else {
                std::cout << "No hay suficiente stock." << std::endl;
            }
        } else if (opcion == 2) {
            // Eliminar todos los detalles de producto
            SQLPrepare(stmt, (SQLCHAR*)"DELETE FROM Detalle_Pedido WHERE Cpedido = ?", SQL_NTS);
            SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &cpedido, 0, NULL);
            SQLExecute(stmt);
            std::cout << "Todos los detalles de producto eliminados." << std::endl;
        } else if (opcion == 3) {
            // Cancelar pedido
            SQLPrepare(stmt, (SQLCHAR*)"DELETE FROM Pedido WHERE Cpedido = ?", SQL_NTS);
            SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &cpedido, 0, NULL);
            SQLExecute(stmt);
            std::cout << "Pedido cancelado." << std::endl;
            break;
        } else if (opcion == 4) {
            // Finalizar pedido
            std::cout << "Pedido finalizado." << std::endl;
            break;
        } else {
            std::cout << "Opción inválida." << std::endl;
        }
    }

    // Liberar recursos
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}

void cerrar_conexion(SQLHDBC dbc) {
    SQLDisconnect(dbc);
    SQLFreeHandle(SQL_HANDLE_DBC, dbc);
}

int main() {
    SQLHDBC dbc;
    conectar_bd(dbc);
    crear_tablas(dbc);
    mostrar_tablas(dbc);
    dar_alta_nuevo_pedido(dbc);
    mostrar_tablas(dbc);
    cerrar_conexion(dbc);
    return 0;
}
