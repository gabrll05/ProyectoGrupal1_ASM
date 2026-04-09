import numpy as np
import matplotlib.pyplot as plt
import time

# ajustando el tamaño de las graficas para que quepan bien en el pdf del reporte
plt.rcParams["figure.figsize"] = [14, 10]
plt.rcParams.update({"font.size": 12})

print("=== INICIANDO EXPERIMENTOS CON FFT ===")

# ==============================================================================
# PARTE 1: FILTRANDO RUIDO CON PSD
# ==============================================================================
print("\n--- Parte 1: Filtrado de Ruido ---")
dt = 0.001
t = np.arange(0, 1, dt)
n = len(t)

# sumamos dos senos y le metemos bastante ruido aleatorio para probar el filtro
f = np.sin(2 * np.pi * 50 * t) + np.sin(2 * np.pi * 120 * t)
f_clean = f.copy()
f = f + 2.5 * np.random.randn(n)

# sacamos la fft y calculamos la densidad espectral de potencia (PSD)
fhat = np.fft.fft(f, n)
PSD = fhat * np.conj(fhat) / n
freq = (1/(dt*n)) * np.arange(n)
L = np.arange(1, np.floor(n/2), dtype="int")

# pasamos a reales para poder comparar las energias
PSD_real = np.real(PSD)
energia_total = np.sum(PSD_real) 

# al final usamos un umbral fijo en vez de % de energia, porque 
# el ruido inicial era demasiado alto y pedirle el 95% no limpiaba nada
umbral = 100 
indices = PSD_real > umbral
num_componentes = np.sum(indices) 

# aplicamos la mascara de frecuencias y nos devolvemos al dominio del tiempo con ifft
PSDclean = PSD * indices
fhat_clean = indices * fhat
ffilt = np.real(np.fft.ifft(fhat_clean))

# calculos para tabular en el reporte (MSE y energia)
energia_preservada = 100 * np.sum(np.real(PSDclean)) / energia_total
mse_ruidosa = np.mean((f_clean - f) ** 2)
mse_filtrada = np.mean((f_clean - ffilt) ** 2)

print(f"MSE señal ruidosa: {mse_ruidosa:.4f}")
print(f"MSE señal filtrada: {mse_filtrada:.4f}")
print(f"Componentes importantes detectadas: {num_componentes}")
print(f"Energía preservada (%): {energia_preservada:.2f}")

# ploteamos las 3 subgraficas de la parte 1
fig1, axs1 = plt.subplots(3, 1)
fig1.suptitle("Parte 1: Filtrado de Ruido usando FFT y PSD", fontsize=16)

axs1[0].plot(t, f, color="c", linewidth=1.5, label="Noisy")
axs1[0].plot(t, f_clean, color="k", linewidth=2, label="Clean")
axs1[0].set_xlim(t[0], t[-1])
axs1[0].legend()

axs1[1].plot(t, ffilt, color="k", linewidth=2, label="Filtered")
axs1[1].set_xlim(t[0], t[-1])
axs1[1].legend()

axs1[2].plot(freq[L], np.real(PSD[L]), color="c", linewidth=2, label="Noisy PSD")
axs1[2].plot(freq[L], np.real(PSDclean[L]), color="k", linewidth=1.5, label="Filtered PSD")
axs1[2].set_xlim(freq[L[0]], freq[L[-1]])
axs1[2].legend()
fig1.tight_layout()


# ==============================================================================
# PARTE 2: NUESTRA DFT VS FFT DE NUMPY
# ==============================================================================
print("\n--- Parte 2: Comparación de Tiempos DFT vs FFT ---")

# implementacion a pata de la dft para comparar la complejidad matematica
def dft_manual(x):
    N_len = len(x)
    n_idx = np.arange(N_len)
    k_idx = n_idx.reshape((N_len, 1))
    # matriz de exponenciales complejas
    e = np.exp(-2j * np.pi * k_idx * n_idx / N_len)
    return np.dot(e, x)

tamaños_N = [64, 128, 256, 512, 1024, 2048, 4096]
tiempos_dft = []
tiempos_fft = []

# corremos las pruebas midiendo el tiempo de cada una con arreglos cada vez mas grandes
for N_val in tamaños_N:
    señal_prueba = np.random.random(N_val)
    
    # midiendo nuestra dft manual O(N^2)
    inicio = time.time()
    dft_manual(señal_prueba)
    tiempos_dft.append(time.time() - inicio)
    
    # midiendo la fft rapida O(N log N)
    inicio = time.time()
    np.fft.fft(señal_prueba)
    tiempos_fft.append(time.time() - inicio)

print("Tiempos de ejecución calculados con éxito.")

# ploteamos la grafica de rendimiento
fig2 = plt.figure(figsize=(10, 6))
plt.plot(tamaños_N, tiempos_dft, 'ro-', label='DFT Manual $O(N^2)$', linewidth=2)
plt.plot(tamaños_N, tiempos_fft, 'bo-', label='FFT Numpy $O(N \\log N)$', linewidth=2)
plt.title('Parte 2: Comparación de Tiempos de Ejecución (DFT vs FFT)', fontsize=16)
plt.xlabel('Número de muestras (N)')
plt.ylabel('Tiempo de ejecución (segundos)')
plt.legend()
plt.grid(True)
fig2.tight_layout()


# ==============================================================================
# PARTE 3: MAGNITUD Y FASE
# ==============================================================================
print("\n--- Parte 3: Análisis de Magnitud y Fase ---")

SR = 1000      
T_dur = 1          
t_fase = np.linspace(0, T_dur, SR * T_dur, endpoint=False)

# probando con una onda de 10hz normal y otra de 20hz movida 90 grados (pi/2)
f1, f2 = 10, 20
desfase = np.pi / 2
y_fase = np.sin(2 * np.pi * f1 * t_fase) + np.sin(2 * np.pi * f2 * t_fase + desfase)

N_fase = len(y_fase)
fhat_fase = np.fft.fft(y_fase)
frecuencias_fase = np.fft.fftfreq(N_fase, 1/SR)

# sacando magnitud y fase de los numeros complejos
magnitud = np.abs(fhat_fase) / N_fase 
fase = np.angle(fhat_fase)

# filtrando la estatica matematica para que la grafica de fase se vea limpia
fase[magnitud < 0.01] = 0 

# cortamos la mitad negativa del espectro que es un reflejo de la positiva
mitad = N_fase // 2
frecuencias_pos = frecuencias_fase[:mitad]
magnitud_pos = magnitud[:mitad] * 2  
fase_pos = fase[:mitad]

print("Análisis de señal compleja completado.")

# graficamos la señal original, su magnitud y su fase
fig3, axs3 = plt.subplots(3, 1, figsize=(12, 10))
fig3.suptitle("Parte 3: Análisis Complejo (Magnitud y Fase)", fontsize=16)

axs3[0].plot(t_fase[:200], y_fase[:200], color='k', linewidth=2) 
axs3[0].set_title('Señal en el Tiempo: $10Hz + 20Hz$ (desfasada $\\pi/2$)')
axs3[0].set_xlabel('Tiempo (s)')
axs3[0].set_ylabel('Amplitud')
axs3[0].grid(True)

axs3[1].stem(frecuencias_pos[:50], magnitud_pos[:50], basefmt=" ")
axs3[1].set_title('Espectro de Magnitud')
axs3[1].set_xlabel('Frecuencia (Hz)')
axs3[1].set_ylabel('Magnitud')
axs3[1].grid(True)

axs3[2].stem(frecuencias_pos[:50], fase_pos[:50], basefmt=" ")
axs3[2].set_title('Espectro de Fase (Radianes)')
axs3[2].set_xlabel('Frecuencia (Hz)')
axs3[2].set_ylabel('Fase')
axs3[2].grid(True)
fig3.tight_layout()

print("\nGeneración terminada. Se mostrarán 3 ventanas con gráficas.")
plt.show()