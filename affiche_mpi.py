import numpy as np
import matplotlib.pyplot as plt

# Charger les données en spécifiant le délimiteur et comment gérer les valeurs manquantes
def load_data(file_path):
    with open(file_path, 'r') as file:
        lines = file.readlines()
    
    # Trouver le nombre maximal de colonnes
    max_columns = max(len(line.split()) for line in lines)
    
    # Remplir les lignes avec un nombre incorrect de colonnes
    formatted_lines = []
    for line in lines:
        columns = line.split()
        if len(columns) < max_columns:
            columns += [np.nan] * (max_columns - len(columns))
        
        # Convertir les colonnes en float, en ignorant les valeurs non numériques
        try:
            formatted_columns = [float(col) for col in columns]
            formatted_lines.append(formatted_columns)
        except ValueError:
            pass
    
    return np.array(formatted_lines)

data = load_data('temperature_mpi.txt')

print("Valeur minimale des données :", np.nanmin(data))
print("Valeur maximale des données :", np.nanmax(data))

# Afficher un graphique de la distribution de température
plt.imshow(data, cmap='hot', interpolation='nearest')
cbar = plt.colorbar(label='Température (°C)', ticks=np.linspace(np.nanmin(data), np.nanmax(data), 5))
cbar.ax.set_yticklabels([str(int(i)) for i in np.linspace(np.nanmin(data), np.nanmax(data), 5)])
plt.title('Distribution de température sur la plaque')
plt.xlabel('Position en X')
plt.ylabel('Position en Y')

# Enregistrer le graphique sous forme de fichier image
plt.savefig('temperature_mpi_plot.png')
