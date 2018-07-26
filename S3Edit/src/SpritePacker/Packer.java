package SpritePacker;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;

/**
 * Created by justin on 1/5/18.
 */
public class Packer {
    public class Node {
        public Node rightNode;
        public Node bottomNode;
        public double pos_x;
        public double pos_y;
        public double width;
        public double height;
        public boolean isOccupied;
        public Node(double w, double l) {
            width = w;
            height = l;
        }
        public Node(double pos_x, double pos_y, double width, double height) {
            this.pos_x = pos_x;
            this.pos_y = pos_y;
            this.width = width;
            this.height = height;
        }
    }

    public static class Box {
        public double width;
        public double height;
        public double volume;
        public Node position;
        public int id;
        public int index;
        public IRectangle2D source;
        public Box(double w, double h, int id) {
            width = w;
            height = h;
            this.id = id;
        }
    }

    public double containerWidth = 512;
    public double containerHeight = 512;
    public ArrayList<Box> _boxes;
    private Node rootNode;

    class CustomComparator implements Comparator<Box> {
        @Override
        public int compare(Box o1, Box o2) {
            return (int)(o2.volume - o1.volume);
        }
    }

    public Packer()
    {
        _boxes = new ArrayList<>();

    }

    public void Do() {
        // Sort boxes into descending order based on volume
        for (Box box : _boxes)
        {
            box.volume = box.height * box.width;
        }
        Collections.sort(_boxes, new CustomComparator());

        // Initialize root node
        rootNode = new Node(containerHeight, containerWidth);

        Pack();
        Display();
    }

    private void Display() {
        for (Box box : _boxes) {
            if (box.position == null) {
                System.out.println("Failed, trying again.");
                //if (containerHeight < 512) {
                    containerHeight *= 2;
                    containerWidth *= 2;
                //}
                //else {
                //    containerWidth += 32;
                //    containerHeight += 32;
                //}
                Do();
                return;
            }
        }
    }

    private void Pack() {
        for (Box box : _boxes) {
            Node node = FindNode(rootNode, box.width, box.height);
            if (node != null) {
                // Split rectangles
                box.position = SplitNode(node, box.width, box.height);
            }
        }
    }

    private Node FindNode(Node rootNode, double boxWidth, double boxHeight) {
        if (rootNode.isOccupied) {
            Node nextNode = FindNode(rootNode.rightNode, boxWidth, boxHeight);

            if (nextNode == null)
            {
                nextNode = FindNode(rootNode.bottomNode, boxWidth, boxHeight);
            }

            return nextNode;
        }
        else if (boxWidth <= rootNode.width && boxHeight <= rootNode.height)
        {
            return rootNode;
        }
        else
        {
            return null;
        }
    }

    private Node SplitNode(Node node, double boxWidth, double boxHeight)
    {
        node.isOccupied = true;
        node.bottomNode = new Node(node.pos_x, node.pos_y + boxHeight, node.width, node.height - boxHeight);
        node.rightNode = new Node(node.pos_x + boxWidth, node.pos_y, node.width - boxWidth, boxHeight);
        return node;
    }
}
