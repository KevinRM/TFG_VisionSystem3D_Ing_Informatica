De aqu�, se import� el paquete de Unity3D. Luego, daba fallos con algunas dll (las que est�n dentro de la carpeta librariesLibreriasCambiadas
es para indicar las que fallaban y que se cambiaron. Si se utilizan esas, va a fallar.). Cada una de esas dll se sustituy� por:
	- Del .zip LibreriaCvSharpExtern: �nicamente la dll OpenCvSharpExtern.dll
	- De la librer�a OpenCV el resto.
En el proyecto de Unity3D, assets, plugins estan las dll que funcionan ya importadas