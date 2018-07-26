package SpritePacker;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;

/**
 * Created by justin on 1/5/18.
 */
public class PackerPlus {
    public class Box {
        public double x;
        public double y;
        public double w;
        public double h;
        public double volume;

        public IRectangle2D source;
    }
    public class Node {
        public Node[] child = new Node[2];

        public int id;
        public int index;

        public double x;
        public double y;
        public double w;
        public double h;
        public boolean isOccupied;
        public double volume;


        public Node(double w, double h, int id) {
            this.w = w;
            this.h = h;
            this.id = id;
        }

        public Node(double w, double h) {
            this.w = w;
            this.h = h;
        }
        public Node(double pos_x, double pos_y, double width, double height) {
            this.x = pos_x;
            this.y = pos_y;
            this.w = width;
            this.h = height;
        }


        private Node Insert(Box img) {
            if (false) {
                Node newNode = child[0].Insert(img);
                if (newNode != null)
                    return newNode;

                return child[1].Insert(img);
            }



            return null;//pnode.child[0].Insert(img);
        }
    }


    public double containerWidth = 512;
    public double containerHeight = 512;
    public ArrayList<Box> _boxes;
    private Node rootNode;

    class CustomComparator implements Comparator<Node> {
        @Override
        public int compare(Node o1, Node o2) {
            return (int)(o2.volume - o1.volume);
        }
    }

    public PackerPlus()
    {
        _boxes = new ArrayList<>();
    }

    public void Do() {
        // Sort boxes into descending order based on volume
        for (Box box : _boxes) {
            box.volume = box.h * box.w;
        }
        //Collections.sort(_boxes, new CustomComparator());

        // Initialize root node
        rootNode = new Node(containerHeight, containerWidth);

        Pack();
    }

    private void Pack()
    {
        for (Box box : _boxes) {
            rootNode.Insert(box);
        }
    }
}
