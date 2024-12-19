function eliminarGrupo(grupoId) {
    if (confirm("¿Estás seguro de que deseas eliminar este grupo?")) {
        apex.server.process(
            "ELIMINAR_GRUPO", // Nombre del proceso en APEX
            { x01: grupoId }, // Envía el grupo ID como parámetro
            {
                success: function() {
                    alert("Grupo eliminado correctamente.");
                    location.reload(); // Recarga la página para actualizar los grupos
                },
                error: function() {
                    alert("Error al eliminar el grupo.");
                }
            }
        );
    }
}

