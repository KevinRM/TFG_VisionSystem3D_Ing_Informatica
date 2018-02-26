using UnityEngine;
using System.Collections;
using UnityEngine.UI;
using System;
using OpenCvSharp;

public class VideoInput : MonoBehaviour {
    // Left Eye
    public Material inputMaterial_leftEye;                          // Material to take the video from the camera and put it on the plane
    private WebCamTexture camText_leftEye;                          // Texture to receive the video image
    private Vector3 positionEyeLeft = new Vector3 (-2158, 0, 0);    // Modify camera position when running
    private Transform transformEyeLeft;                                // Check the position of the eye
    // Right Eye
    public Material inputMaterial_rightEye;                         // Material to take the video from the camera and put it on the plane
    private WebCamTexture camText_rightEye;                         // Texture to receive the video image
    private Vector3 positionEyeRight = new Vector3 (2158, 0, 0);    // Modify camera position when running

    // Parameters to edit images
    private string pathRmapsFile = "FilesVS3D/rmaps.yml";
    //private string pathRmapsFile = "./Assets/FilesCameraParam/rmaps.yml";
    private CvMat[,] rmap = new CvMat[2, 2];
    private int imgWidth;
    private int imgHeight;
    private bool canEditVideo = false;

    void Start() {
        WebCamDevice[] devices = WebCamTexture.devices;
        Debug.Log ("Detecto " + devices.Length + " dispositivos conectados");

        if (devices.Length < 2) {
            GameObject.Find ("Error").GetComponent<Text> ().enabled = true;
        } else {
            // Read Rmaps to modify the images
            bool correct = readRmapParameters ();

            if (correct) {
                // Left Eye
                camText_leftEye = new WebCamTexture ();
                camText_leftEye.deviceName = devices[0].name;
                camText_leftEye.Play ();
                inputMaterial_leftEye.mainTexture = camText_leftEye;
                // Right Eye
                camText_rightEye = new WebCamTexture ();
                camText_rightEye.deviceName = devices[1].name;
                camText_rightEye.Play ();
                inputMaterial_rightEye.mainTexture = camText_rightEye;

                Invoke ("adjustPositionEyes", 1f);

                // Take image camera size to create CvMat object
                imgWidth = camText_leftEye.width;
                imgHeight = camText_leftEye.height;

                Invoke ("setCanEditVideo", 3f);
            }
        }
    }

    private void adjustPositionEyes() {
        if (!canEditVideo) {
            transformEyeLeft = GameObject.Find ("Main Camera Left").transform;
        }
        transformEyeLeft.position = positionEyeLeft;
        GameObject.Find ("Main Camera Right").transform.position = positionEyeRight;
    }

    private bool readRmapParameters() {
        // Read M00
        try {
            CvFileStorage read = new CvFileStorage (pathRmapsFile, null, FileStorageMode.Read);
            CvFileNode param = read.GetFileNodeByName (null, "M00");
            rmap[0, 0] = read.Read<CvMat> (param);

            // Read M01
            read = new CvFileStorage (pathRmapsFile, null, FileStorageMode.Read);
            param = read.GetFileNodeByName (null, "M01");
            rmap[0, 1] = read.Read<CvMat> (param);

            // Read M10
            read = new CvFileStorage (pathRmapsFile, null, FileStorageMode.Read);
            param = read.GetFileNodeByName (null, "M10");
            rmap[1, 0] = read.Read<CvMat> (param);

            // Read M11
            read = new CvFileStorage (pathRmapsFile, null, FileStorageMode.Read);
            param = read.GetFileNodeByName (null, "M11");
            rmap[1, 1] = read.Read<CvMat> (param);
            return true;
        } catch (Exception e) {
            GameObject.Find ("Error").GetComponent<Text> ().enabled = true;
            GameObject.Find ("Error").GetComponent<Text> ().text = "Error al leer el archivo rmap " + e;
            return false;
        }
    }

    private void setCanEditVideo() {
        canEditVideo = true;
    }

    private void Update() {
        if (canEditVideo) {
            StartCoroutine ("editStreamingVideo");
        }
        if (canEditVideo && (transformEyeLeft.position != positionEyeLeft)) {
            adjustPositionEyes ();
        }
    }

    private IEnumerator editStreamingVideo() {
        // Convert image to CvMat
        Color[] pixels_left = camText_leftEye.GetPixels ();
        Color[] pixels_right = camText_rightEye.GetPixels ();
        CvMat imageConvertedLeft = new CvMat (imgHeight, imgWidth, MatrixType.U8C3);
        CvMat imageConvertedRight = new CvMat (imgHeight, imgWidth, MatrixType.U8C3);

        //CvMat map1 = new CvMat (imgHeight, imgWidth, MatrixType.S16C2);
        //CvMat map2 = new CvMat (imgHeight, imgWidth, MatrixType.U16C1);
        for (int i = 0; i < imgHeight; i++) {
            for (int j = 0; j < imgWidth; j++) {
                Color pixel_left = pixels_left[j + i * imgWidth];
                Color pixel_right = pixels_right[j + i * imgWidth];
                CvScalar scalar_left = new CvScalar (pixel_left.b * 255, pixel_left.g * 255, pixel_left.r * 255);
                CvScalar scalar_right = new CvScalar (pixel_right.b * 255, pixel_right.g * 255, pixel_right.r * 255);
                imageConvertedLeft.Set2D ((imgHeight - 1) - i, (imgWidth - 1) - j, scalar_left);
                imageConvertedRight.Set2D ((imgHeight - 1) - i, (imgWidth - 1) - j, scalar_right);
                //map1.Set2D (i, j, new CvScalar ((imgWidth - 1) - j));
                //map2.Set2D (i, j, new CvScalar ((imgHeight - 1) - i));
            }
        }

        // Remapping
        CvMat outImg_left = new CvMat (imgHeight, imgWidth, MatrixType.U8C3);
        CvMat outImg_right = new CvMat (imgHeight, imgWidth, MatrixType.U8C3);
        Cv.Remap (imageConvertedLeft, outImg_left, rmap[0, 0], rmap[0, 1], Interpolation.Linear);
        Cv.Remap (imageConvertedRight, outImg_right, rmap[1, 0], rmap[1, 1], Interpolation.Linear);

        // Convert image to Texture2D
        IplImage converted_left = Cv.GetImage (outImg_left);
        IplImage converted_right = Cv.GetImage (outImg_right);
        Texture2D videoTexture_left = new Texture2D (imgWidth, imgHeight, TextureFormat.RGB24, false);
        Texture2D videoTexture_right = new Texture2D (imgWidth, imgHeight, TextureFormat.RGB24, false);
        for (int i = 0; i < imgWidth; i++) {
            for (int j = 0; j < imgHeight; j++) {
                float b_left = (float) converted_left[j, i].Val0;
                float g_left = (float) converted_left[j, i].Val1;
                float r_left = (float) converted_left[j, i].Val2;
                float b_right = (float) converted_right[j, i].Val0;
                float g_right = (float) converted_right[j, i].Val1;
                float r_right = (float) converted_right[j, i].Val2;
                Color color_left = new Color (r_left / 255.0f, g_left / 255.0f, b_left / 255.0f);
                Color color_right = new Color (r_right / 255.0f, g_right / 255.0f, b_right / 255.0f);
                videoTexture_left.SetPixel (imgWidth - i, imgHeight - j, color_left);
                videoTexture_right.SetPixel (imgWidth - i, imgHeight - j, color_right);
            }
        }
        videoTexture_left.Apply ();
        videoTexture_right.Apply ();
        inputMaterial_leftEye.mainTexture = videoTexture_left;
        inputMaterial_rightEye.mainTexture = videoTexture_right;
        yield return null;
    }
}