import pandas as pd  
import numpy as np
import matplotlib.pyplot as plt

# Read data from csv file
df = pd.read_csv('friis_pre_experiment.csv')
df_friis = pd.read_csv('friis.csv')
df_fixed = pd.read_csv('fixed.csv')
df_nakagami = pd.read_csv('nakagami.csv')
df_threelog = pd.read_csv('threelog.csv')
df_tworay = pd.read_csv('tworay.csv')

x_daten = df_friis['distance']
x_daten2 = df_fixed['distance']
x_daten3 = df_nakagami['distance']
x_daten4 = df_threelog['distance']
x_daten5 = df_tworay['distance']

y_daten = df_friis['troughput']
y_daten2 = df_fixed['troughput']
y_daten3 = df_nakagami['troughput']
y_daten4 = df_threelog['troughput']
y_daten5 = df_tworay['troughput']


plt.plot(x_daten, y_daten, linestyle='-', label='Friis')
plt.plot(x_daten2, y_daten2, linestyle='-', label='Fixed')
plt.plot(x_daten3, y_daten3,  linestyle='-', label='Nakagami')
plt.plot(x_daten4, y_daten4, linestyle='-', label='ThreeLog')
plt.plot(x_daten5, y_daten5, linestyle='-', label='TwoRay')

plt.xlabel('Distance [m]')
plt.ylabel('Data Layer Troughput[Mbit/s]')
plt.legend()
plt.xlim(1,260)
plt.ylim(30,80)
plt.show()




y_daten = df_friis['signal_mean']
y_daten2 = df_fixed['signal_mean']
y_daten3 = df_nakagami['signal_mean']
y_daten4 = df_threelog['signal_mean']
y_daten5 = df_tworay['signal_mean']
y_daten_noise = df_threelog['noise']

plt.plot(x_daten, y_daten, linestyle='-', label='Friis')
plt.plot(x_daten2, y_daten2, linestyle='-', label='Fixed')
plt.plot(x_daten3, y_daten3,  linestyle='-', label='Nakagami')
plt.plot(x_daten4, y_daten4, linestyle='-', label='ThreeLog')
plt.plot(x_daten5, y_daten5, linestyle='-', label='TwoRay')
plt.plot(x_daten4, y_daten_noise, linestyle='-', label='Avg. Noise Lvl.')



plt.xlabel('Distance[m]')
plt.ylabel('RSSI[dBm]')
plt.legend()
plt.xlim(1,250)
plt.ylim(-94,10)
plt.show()