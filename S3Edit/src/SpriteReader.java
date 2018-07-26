import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

/**
 * Created by Justin on 2/18/2018.
 */
public class SpriteReader {
    public static void main(String[] args) {
        String fileName = "C:\\Users\\Justin\\Saved Games\\Hacking\\Sonic Mania\\Data\\Sprites\\Players\\Sonic.bin";

        DataInputStream in = null;
        FileOutputStream out = null;

        try {
            in = new DataInputStream(new FileInputStream(fileName));
            out = new FileOutputStream("ManiaTailssdsdfsdf.spr");

            //System.out.println("Magic Word: " + readAsString(in, 4));

            int v = 0;

            read(in, 0x5E - 2);
            int count = readWord(in);
            for (int a = 0; a < count; a++) {
                System.out.println("\"" + readRSDKString(in) + "\" (index: " + v + ")");
                int frameCount = readWord(in);

                System.out.println(" - Animation Speed: " + readWord(in) + "");
                read(in);
                read(in);

                /*
                System.out.println(" - Frame Count: " + frameCount + "");
                System.out.println(" - Animation Speed: " + readWord(in) + "");
                System.out.println(" - Frame to Loop: " + read(in) + "");
                System.out.println("      " + read(in) + "");
                //*/

                System.out.print(" - [");
                for (int i = 0; i < frameCount; i++) {
                    int val = read(in, 4)[0];
                    if (i == 0 || i == frameCount - 1) { // Print first and last
                        if (i > 0)
                            System.out.print(", ");
                        System.out.print(v);
                    }
                    int sheetNum = read(in);
                    /*
                    System.out.println(" - Frame #" + i + ": ");
                    System.out.println("      Sheet Number: " + read(in) + "");
                    //*/
                    int X = readWord(in);
                    int Y = readWord(in);
                    int W = readWord(in);
                    int H = readWord(in);
                    int OffX = (short)readWord(in);
                    int OffY = (short)readWord(in);

                    if (val != 0) {
                        //System.out.print(" (" + val + ")");
                    }

                    X += 512 * (val % 2);
                    Y += 512 * (val / 2);

                    /*
                    System.out.println("      X: " + X + "");
                    System.out.println("      Y: " + Y + "");
                    System.out.println("      W: " + W + "");
                    System.out.println("      H: " + H + "");
                    System.out.println("      OffX: " + OffX + "");
                    System.out.println("      OffY: " + OffY + "");
                    System.out.println("      OffCenterX: " + (64 / 2 + OffX) + "");
                    System.out.println("      OffCenterY: " + (64 / 2 + OffY) + "");
                    //*/

                    int OX = (64 / 2 + OffX);
                    int OY = (64 / 2 + OffY);

                    // X
                    out.write((X / 0x100) & 0xFF);
                    out.write((X / 0x001) & 0xFF);
                    // Y
                    out.write((Y / 0x100) & 0xFF);
                    out.write((Y / 0x001) & 0xFF);
                    // W
                    out.write((W / 0x100) & 0xFF);
                    out.write((W / 0x001) & 0xFF);
                    // H
                    out.write((H / 0x100) & 0xFF);
                    out.write((H / 0x001) & 0xFF);
                    // OX
                    out.write((OX / 0x001) & 0xFF);
                    // OY
                    out.write((OY / 0x001) & 0xFF);
                    // OW
                    out.write(0x00);
                    out.write(0x40);
                    // OH
                    out.write(0x00);
                    out.write(0x40);

                    // File.OffX = W / 2 + OffX
                    // File.OffY = H / 2 + OffY
                    // File.OffW = W
                    // File.OffH = H

                    read(in, 16);
                    /*
                    System.out.println("      Object Hitbox X1: " + (short) readWord(in) + "");
                    System.out.println("      Object Hitbox Y1: " + (short) readWord(in) + "");
                    System.out.println("      Object Hitbox X2: " + (short) readWord(in) + "");
                    System.out.println("      Object Hitbox Y2: " + (short) readWord(in) + "");
                    System.out.println("      Stage Hitbox X1: " + (short) readWord(in) + "");
                    System.out.println("      Stage Hitbox Y1: " + (short) readWord(in) + "");
                    System.out.println("      Stage Hitbox X2: " + (short) readWord(in) + "");
                    System.out.println("      Stage Hitbox Y2: " + (short) readWord(in) + "");
                    */

                    //System.out.println("      End: 0x" + Integer.toHexString(readWord(in)) + "");

                    v++;
                }
                System.out.println("]");
            }

            if (in != null) {
                in.close();
            }
            if (out != null) {
                out.close();
            }
        }
        catch (Exception e) {
            System.out.println(e.getMessage());
        }
    }

    public static int[] read(DataInputStream in, int n) throws IOException {
        int[] arr = new int[n];
        for (int i = 0; i < n; i++) {
            arr[i] = in.readUnsignedByte();
        }
        return arr;
    }
    public static int   read(DataInputStream in) throws IOException {
        return in.readUnsignedByte();
    }
    public static int   readWord(DataInputStream in) throws IOException {
        return in.readUnsignedByte() + in.readUnsignedByte() * 0x100;
    }
    public static String readAsString(DataInputStream in, int n) throws IOException {
        String stringFinal = "";
        for (int i = 0; i < n; i++) {
            stringFinal += (char)in.readUnsignedByte();
        }
        return stringFinal;
    }
    public static String readRSDKString(DataInputStream in) throws IOException {
        String stringFinal = "";
        int stringLength = in.readUnsignedByte();

        for (int i = 0; i < stringLength - 1; i++) {
            stringFinal += (char)in.readUnsignedByte();
        }
        in.readUnsignedByte();

        return stringFinal;
    }
}
