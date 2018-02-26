using OpenHMDNet;
using System;
using System.Collections.Generic;
using System.IO.Ports;

public class EulerAnglesStruct {
    public double x;     /* rotation around x axis in degrees */
    public double y;    /* rotation around y axis in degrees */
    public double z;      /* rotation around z axis in degrees */
};

public class Gyro {
    SerialPort ArduinoPort;
    double[] q = new double[4];
    double prevX;
    double currX;
    int canSendData = 0;
    double angleRotation = 80;
    double prevY;
    double currY;
    double angleInclination = 80;

    Gyro() {
        ArduinoPort = new SerialPort ();
        ArduinoPort.PortName = "COM5";
        ArduinoPort.BaudRate = 115200;
        ArduinoPort.Open ();
        // Gafas
        OpenHMD driver = new OpenHMD ();
        driver.openContext ();
        List<Device> devices = driver.getDevices ();
        Device selectedDevice = null;

        foreach (Device dev in devices) {
            Console.WriteLine ("Device: " + dev.Index);
            Console.WriteLine ("  Vendor: " + dev.Vendor);
            Console.WriteLine ("  Product: " + dev.Product);
            Console.WriteLine ("  Path: " + dev.Path);

            selectedDevice = dev;
        }

        driver.openDevice (selectedDevice);

        driver.update ();
        float[] rotation = driver.getFloatInformation (selectedDevice, 4, OpenHMD.ohmd_float_value.OHMD_ROTATION_QUAT);
        quaternion (rotation[0], rotation[1], rotation[2], rotation[3]);
        EulerAnglesStruct euler = getEulerAngles ();
        prevX = euler.x;
        prevY = euler.y;
        // Initialize position of servos
        ArduinoPort.Write ("r" + sendAngle(angleRotation));
        ArduinoPort.Write ("i" + sendAngle(angleInclination));

        while (true) {
            driver.update ();
            rotation = driver.getFloatInformation (selectedDevice, 4, OpenHMD.ohmd_float_value.OHMD_ROTATION_QUAT);
            quaternion (rotation[0], rotation[1], rotation[2], rotation[3]);
            euler = getEulerAngles ();

            currX = euler.x;
            currY = euler.y;

            //Console.WriteLine ("rotation euler-> x: " + Math.Abs (prevX - currX) + "y: " + Math.Abs (prevY - currY));
            if (currX > prevX && Math.Abs (prevX - currX) >= 0.09d) {
                //if (angleRotation < 180) {
                angleRotation += 0.6;
                //Console.WriteLine ("Rotation: " + angleRotation);
                ArduinoPort.Write ("r" + sendAngle (angleRotation));
                //Console.WriteLine (angleRotation);
                //ArduinoPort.Write ("r");
                //canSendData++;
                //angleRotation += 1;
                /*sendAngle = angleRotation.ToString ();
                if (sendAngle.Length < 3) {
                    sendAngle = "0" + sendAngle;
                }
                if (canSendData == 3) {
                    canSendData = 0;
                    ArduinoPort.Write ("r" + sendAngle);
                    Console.WriteLine (angleRotation);
                }*/
                //}
            }
            if (currX < prevX && Math.Abs (prevX - currX) >= 0.13d) {
                //ArduinoPort.Write ("a" + angleRotation);
                //if (angleRotation > 2) {
                angleRotation -= 0.6;
                //Console.WriteLine ("Rotation: " + angleRotation);
                ArduinoPort.Write ("r" + sendAngle (angleRotation));
                //Console.WriteLine (angleRotation);
                //ArduinoPort.Write ("l");
                /*canSendData++;
                angleRotation -= 1;
                string sendAngle = angleRotation.ToString ();
                if (sendAngle.Length < 3) {
                    sendAngle = "0" + sendAngle;
                }
                if (canSendData == 3) {
                    canSendData = 0;
                    ArduinoPort.Write ("r" + sendAngle);
                    Console.WriteLine (angleRotation);
                }*/
                //}
            }
            if (currY > prevY && Math.Abs (prevY - currY) >= 0.08d) {
                //Console.WriteLine ("rotation euler-> x: " + euler.x + " y:" + euler.y + " z:" + euler.z);
                //ArduinoPort.Write ("s" + angleInclination);
                //ArduinoPort.Write ("u");
                angleInclination += 0.6;
                //Console.WriteLine ("Inclination: " + angleInclination);
                ArduinoPort.Write ("i" + sendAngle (angleInclination));
            }
            if (currY < prevY && Math.Abs (prevY - currY) >= 0.09d) {
                //ArduinoPort.Write ("w" + angleInclination);
                //ArduinoPort.Write ("d");
                angleInclination -= 0.6;
                //Console.WriteLine ("Inclination: " + angleInclination);
                ArduinoPort.Write ("i" + sendAngle (angleInclination));
            }
            

            prevX = currX;
            prevY = currY;

            System.Threading.Thread.Sleep (10);
        }

        driver.closeContext ();
    }

    private string sendAngle(double angle) {
        string returnAngle = ((int)angle).ToString ();
        while (returnAngle.Length < 3) {
            returnAngle = "0" + returnAngle;
        }
        return returnAngle;
    }

    private void quaternion(float w, float x, float y, float z) {
        q[0] = w;
        q[1] = x;
        q[2] = y;
        q[3] = z;
    }

    private EulerAnglesStruct getEulerAngles() {
        EulerAnglesStruct eulerAnglesStruct = new EulerAnglesStruct();
        eulerAnglesStruct.x = radiansToDegrees (Math.Atan2(2.0d * (q[2] * q[3] - q[0] * q[1]), 2.0d * q[0] * q[0] - 1.0d + 2.0d * q[3] * q[3]));
        eulerAnglesStruct.y = radiansToDegrees (-Math.Atan ((2.0d * (q[1] * q[3] + q[0] * q[2])) / Math.Sqrt (1.0d - Math.Pow ((2.0d * q[1] * q[3] + 2.0d * q[0] * q[2]), 2.0d))));
        eulerAnglesStruct.z = radiansToDegrees (Math.Atan2 (2.0d * (q[1] * q[2] - q[0] * q[3]), 2.0d * q[0] * q[0] - 1.0d + 2.0d * q[1] * q[1]));
        return eulerAnglesStruct;
    }

    private double radiansToDegrees(double radians) {
        return 57.2957795130823f * radians;
    }


static void Main(string[] args) {
        new Gyro ();
    }
}

/*namespace OpenHMDExample
{
    class Program
    {
        static void Main(string[] args)
        {
            OpenHMD driver = new OpenHMD();
            driver.openContext();

            List<Device> devices = driver.getDevices();
            Device selectedDevice = null;

            foreach (Device dev in devices)
            {
                Console.WriteLine("Device: " + dev.Index);
                Console.WriteLine("  Vendor: " + dev.Vendor);
                Console.WriteLine("  Product: " + dev.Product);
                Console.WriteLine("  Path: " + dev.Path);

                selectedDevice = dev;
            }

            driver.openDevice(selectedDevice);
        
            Console.WriteLine("Horizontal Resolution: " + driver.getIntInformation(selectedDevice, OpenHMD.ohmd_int_value.OHMD_SCREEN_HORIZONTAL_RESOLUTION));
            Console.WriteLine("Vertical   Resolution: " + driver.getIntInformation(selectedDevice, OpenHMD.ohmd_int_value.OHMD_SCREEN_VERTICAL_RESOLUTION));

            Console.WriteLine("hsize: " + driver.getFloatInformation(selectedDevice, 1, OpenHMD.ohmd_float_value.OHMD_SCREEN_HORIZONTAL_SIZE)[0]);
            Console.WriteLine("vsize: " + driver.getFloatInformation(selectedDevice, 1, OpenHMD.ohmd_float_value.OHMD_SCREEN_VERTICAL_SIZE)[0]);
            Console.WriteLine("lens separation: " + driver.getFloatInformation(selectedDevice, 1, OpenHMD.ohmd_float_value.OHMD_LENS_HORIZONTAL_SEPARATION)[0]);
            Console.WriteLine("lens vcenter: " + driver.getFloatInformation(selectedDevice, 1, OpenHMD.ohmd_float_value.OHMD_LENS_VERTICAL_POSITION)[0]);
            Console.WriteLine("left eye fov: " + driver.getFloatInformation(selectedDevice, 1, OpenHMD.ohmd_float_value.OHMD_LEFT_EYE_FOV)[0]);
            Console.WriteLine("right eye fov: " + driver.getFloatInformation(selectedDevice, 1, OpenHMD.ohmd_float_value.OHMD_RIGHT_EYE_FOV)[0]);
            Console.WriteLine("left eye aspect: " + driver.getFloatInformation(selectedDevice, 1, OpenHMD.ohmd_float_value.OHMD_LEFT_EYE_ASPECT_RATIO)[0]);
            Console.WriteLine("right eye aspect: " + driver.getFloatInformation(selectedDevice, 1, OpenHMD.ohmd_float_value.OHMD_RIGHT_EYE_ASPECT_RATIO)[0]);

            float[] distortion = driver.getFloatInformation(selectedDevice, 6, OpenHMD.ohmd_float_value.OHMD_DISTORTION_K);
            Console.Write("distortion k: ");
            foreach (float fl in distortion)
                Console.Write(fl + " ");
            Console.Write("\r\n");

            while (true)
            {
                driver.update();
                float[] rotation = driver.getFloatInformation(selectedDevice, 4, OpenHMD.ohmd_float_value.OHMD_ROTATION_QUAT);

                Console.Write("rotation quat: ");
                foreach (float fl in rotation)
                    Console.Write(fl + " ");
                Console.Write("\r\n");
                System.Threading.Thread.Sleep(500);
            }

            driver.closeContext();

            Console.ReadLine();
        }
    }
}*/
