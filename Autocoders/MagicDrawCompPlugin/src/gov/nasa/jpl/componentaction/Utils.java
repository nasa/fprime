package gov.nasa.jpl.componentaction;

import java.io.File;

import com.nomagic.magicdraw.core.Application;

/**
 * This class is used by the JFileChooser to just list files with the *.aadl extension
 * @author kevensen
 *
 */

public class Utils {

	/**
	 * Appends to front "===> FATAL Exception: " to the input then prints and throws Connector Exception
	 * 
	 * @param input Input String 
	 * @throws ConnectorException
	 */
	public static void throwConnectorException(String input) throws ConnectorException{
		System.out.println("===> FATAL Exception:" + input);
		throw new ConnectorException(input);
	}
	
	/**
	 * Appends "===> Warning: " to the input then prints the input to standard out and the MagicDraw console
	 * @param input Input String
	 * @throws ConnectorException
	 */
	public static void notifyWarning(String input) throws ConnectorException{
		input = "===> Warning: " + input;
		printToAll(input);
	}
   
    public final static String aadl = "aadl";

    /**
     * Simple utility method to get the file extension of a file.
     * 
     * @param f File to be checked
     * @return String value extension of the file 
     */
    public static String getExtension(File f) {
        String ext = null;
        String s = f.getName();
        int i = s.lastIndexOf('.');

        if (i > 0 &&  i < s.length() - 1) {
            ext = s.substring(i+1).toLowerCase();
        }
        return ext;
    }
    
    /**
     * Combines printing to console out on the Java IDE and MagicDraw
     * 
     * @param s String to be printed
     */
    public static void printToAll(String s){
    	System.out.println(s);
    	Application.getInstance().getGUILog().log(s);
    }
    
    
    /**
     * Generic Pair class that can be used to hold two items of separate types
     *
     * @param <A>
     * @param <B>
     */
    class Pair<A , B>{
    	public A first;
    	public B second;
    	
    	public Pair(final A firstObj  , final B secondObj){
    		this.first = firstObj;
    		this.second = secondObj;
    	}

		@Override
		public int hashCode() {
			final int prime = 31;
			int result = 1;
			result = prime * result + getOuterType().hashCode();
			result = prime * result + ((first == null) ? 0 : first.hashCode());
			result = prime * result + ((second == null) ? 0 : second.hashCode());
			return result;
		}

		@Override
		public boolean equals(Object obj) {
			if (this == obj)
				return true;
			if (obj == null)
				return false;
			if (getClass() != obj.getClass())
				return false;
			Pair other = (Pair) obj;
			if (!getOuterType().equals(other.getOuterType()))
				return false;
			if (first == null) {
				if (other.first != null)
					return false;
			} else if (!first.equals(other.first))
				return false;
			if (second == null) {
				if (other.second != null)
					return false;
			} else if (!second.equals(other.second))
				return false;
			return true;
		}

		private Utils getOuterType() {
			return Utils.this;
		}

		public A getFirst() {
			return first;
		}

		public B getSecond() {
			return second;
		}

		public void setFirst(A first) {
			this.first = first;
		}

		public void setSecond(B second) {
			this.second = second;
		}
    	
    	
    }
}

