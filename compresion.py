import numpy as np
import matplotlib.pyplot as plt

plt.rcParams["figure.figsize"] = [16, 12]
plt.rcParams.update({"font.size": 18})

dt = 0.001
t = np.arange(0, 1, dt)

f = np.sin(2 * np.pi * 50 * t) + np.sin(2 * np.pi * 120 * t)
f_clean = f.copy()
f = f + 2.5 * np.random.randn(len(t))

plt.plot(t, f, color="c", linewidth=1.5, label="Noisy")
plt.plot(t, f_clean, color="k", linewidth=2, label="Clean")

plt.xlim(t[0], t[-1])
plt.legend()


#Aplicacion de fft

n = len(t)
fhat = np.fft.fft(f,n)
PSD = fhat * np.conj(fhat) / n
freq = (1/(dt*n)) * np.arange(n)
L = np.arange(1,np.floor(n/2),dtype="int")

fig,axs = plt.subplots(2,1)

plt.sca(axs[0])
plt.plot(t,f,color = "c", linewidth=1.5,label = "Noisy")
plt.plot(t,f_clean,color="k", linewidth=2,label="clean")
plt.xlim(t[0],t[-1])
plt.legend()

plt.sca(axs[1])
plt.plot(freq[L],PSD[L],color = "c",linewidth = 2,label ="Noisy")
plt.xlim(freq[L[0]],freq[L[-1]])
plt.legend()

energia_total = np.sum(np.real(PSD))
PSD_real = np.real(PSD)

orden = np.argsort(PSD_real)[::-1]
energia_acumulada = np.cumsum(PSD_real[orden])

num_componentes = np.argmax(energia_acumulada >= 0.95 * energia_total) + 1

indices = np.zeros(n, dtype=bool)
indices[orden[:num_componentes]] = True

PSDclean = PSD * indices
fhat = indices * fhat
ffilt = np.fft.ifft(fhat)

energia_preservada = 100 * np.sum(np.real(PSDclean)) / energia_total

fig,axs = plt.subplots(3,1)

plt.sca(axs[0])
plt.plot(t,f,color="c",linewidth=1.5,label = "Noisy")
plt.plot(t,f_clean,color="k",linewidth=2,label = "Clean")
plt.xlim(t[0],t[-1])
plt.legend()

plt.sca(axs[1])
plt.plot(t,ffilt,color = "k",linewidth =2, label= "Filtered")
plt.xlim(t[0],t[-1])
plt.legend()

plt.sca(axs[2])
plt.plot(freq[L],PSD[L],color = "c",linewidth=2,label="Noisy")
plt.plot(freq[L],PSDclean[L],color = "k",linewidth=1.5,label = "Filtered")
plt.xlim(freq[L[0]],freq[L[-1]])
plt.legend()

ffilt = np.real(ffilt)

mse_ruidosa = np.mean((f_clean - f) ** 2)
mse_filtrada = np.mean((f_clean - ffilt) ** 2)

print("MSE señal ruidosa:", mse_ruidosa)
print("MSE señal filtrada:", mse_filtrada)
print("Componentes necesarias para preservar al menos 95% de la energía:", num_componentes)
print("Energía preservada (%):", energia_preservada)

plt.show()