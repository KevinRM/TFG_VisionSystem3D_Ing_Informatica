De aquí, se importó el paquete de Unity3D. Luego, daba fallos con algunas dll (las que están dentro de la carpeta librariesLibreriasCambiadas
es para indicar las que fallaban y que se cambiaron. Si se utilizan esas, va a fallar.). Cada una de esas dll se sustituyó por:
	- Del .zip LibreriaCvSharpExtern: únicamente la dll OpenCvSharpExtern.dll
	- De la librería OpenCV el resto.
En el proyecto de Unity3D, assets, plugins estan las dll que funcionan ya importadas