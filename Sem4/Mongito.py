import pymongo
from pymongo import MongoClient
import tkinter as tk
from tkinter import messagebox
from tkinter import ttk

# Conectar a MongoDB
client = MongoClient("mongodb://localhost:27017/")  # URL por defecto de MongoDB
db = client["empresa"]  # Crear o acceder a la base de datos llamada 'empresa'
clientes = db["clientes"]  # Crear o acceder a la colección llamada 'clientes'

# Funciones para la gestión de clientes

def agregar_cliente():
    nombre = entry_nombre.get()
    email = entry_email.get()
    if nombre and email:
        cliente = {"nombre": nombre, "email": email}
        clientes.insert_one(cliente)  # Insertar un cliente en la colección
        messagebox.showinfo("Éxito", "Cliente agregado exitosamente")
    else:
        messagebox.showerror("Error", "Por favor, ingresa todos los campos")

def modificar_cliente():
    nombre = entry_nombre.get()
    nuevo_email = entry_email.get()
    if nombre and nuevo_email:
        result = clientes.update_one({"nombre": nombre}, {"$set": {"email": nuevo_email}})
        if result.matched_count > 0:
            messagebox.showinfo("Éxito", "Cliente modificado exitosamente")
        else:
            messagebox.showerror("Error", "Cliente no encontrado")
    else:
        messagebox.showerror("Error", "Por favor, ingresa todos los campos")

def eliminar_cliente():
    nombre = entry_nombre.get()
    if nombre:
        result = clientes.delete_one({"nombre": nombre})
        if result.deleted_count > 0:
            messagebox.showinfo("Éxito", "Cliente eliminado exitosamente")
        else:
            messagebox.showerror("Error", "Cliente no encontrado")
    else:
        messagebox.showerror("Error", "Por favor, ingresa el nombre del cliente")

def borrar_tabla():
    confirm = messagebox.askyesno("Confirmar", "¿Estás seguro de que quieres borrar toda la tabla de clientes?")
    if confirm:
        clientes.drop()  # Eliminar toda la colección
        messagebox.showinfo("Éxito", "Colección 'clientes' borrada exitosamente")

def mostrar_tabla():
    # Limpiar los elementos previos en la tabla
    for row in tree.get_children():
        tree.delete(row)
    
    # Obtener todos los clientes de la base de datos
    for cliente in clientes.find():
        # Agregar los datos de cada cliente a la tabla
        tree.insert("", "end", values=(cliente["nombre"], cliente["email"]))

# Crear la ventana principal de la interfaz gráfica
ventana = tk.Tk()
ventana.title("Gestión de Clientes")
ventana.geometry("450x700")  # Tamaño de la ventana

# Crear los widgets para la interfaz gráfica
label_nombre = tk.Label(ventana, text="Nombre:", font=("Arial", 14))
label_nombre.grid(row=0, column=0, pady=10)

entry_nombre = tk.Entry(ventana, font=("Arial", 14))
entry_nombre.grid(row=0, column=1, pady=10)

label_email = tk.Label(ventana, text="Email:", font=("Arial", 14))
label_email.grid(row=1, column=0, pady=10)

entry_email = tk.Entry(ventana, font=("Arial", 14))
entry_email.grid(row=1, column=1, pady=10)

# Botones para las operaciones con mayor tamaño
boton_agregar = tk.Button(ventana, text="Agregar Cliente", command=agregar_cliente, font=("Arial", 14), width=20, height=2)
boton_agregar.grid(row=2, column=0, columnspan=2, pady=10)

boton_modificar = tk.Button(ventana, text="Modificar Cliente", command=modificar_cliente, font=("Arial", 14), width=20, height=2)
boton_modificar.grid(row=3, column=0, columnspan=2, pady=10)

boton_eliminar = tk.Button(ventana, text="Eliminar Cliente", command=eliminar_cliente, font=("Arial", 14), width=20, height=2)
boton_eliminar.grid(row=4, column=0, columnspan=2, pady=10)

boton_borrar_tabla = tk.Button(ventana, text="Borrar Tabla de Clientes", command=borrar_tabla, font=("Arial", 14), width=20, height=2)
boton_borrar_tabla.grid(row=5, column=0, columnspan=2, pady=10)

boton_mostrar_tabla = tk.Button(ventana, text="Mostrar Tabla de Clientes", command=mostrar_tabla, font=("Arial", 14), width=20, height=2)
boton_mostrar_tabla.grid(row=6, column=0, columnspan=2, pady=10)

# Crear el Treeview para mostrar la tabla de clientes
tree = ttk.Treeview(ventana, columns=("Nombre", "Email"), show="headings", height=6)
tree.heading("Nombre", text="Nombre")
tree.heading("Email", text="Email")
tree.grid(row=7, column=0, columnspan=2, pady=10)

# Ejecutar la ventana
ventana.mainloop()

