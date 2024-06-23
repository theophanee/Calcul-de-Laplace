import numpy as np
import matplotlib.pyplot as plt


data = np.loadtxt('temperature.txt', encoding='utf-8')

# Afficher un graphique de la distribution de température
plt.imshow(data, cmap='hot', interpolation='nearest')
plt.colorbar(label='Température (°C)')
plt.title('Distribution de température sur la plaque')
plt.xlabel('Position en X')
plt.ylabel('Position en Y')

# Enregistrer le graphique sous forme de fichier image
plt.savefig('temperature_plot.png')
