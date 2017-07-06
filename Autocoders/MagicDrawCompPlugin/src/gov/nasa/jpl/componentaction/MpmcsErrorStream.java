package gov.nasa.jpl.componentaction;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.OutputStream;
import java.io.PrintStream;
import java.io.UnsupportedEncodingException;

public class MpmcsErrorStream extends PrintStream {

	private PrintStream out;
	
	public MpmcsErrorStream(PrintStream out1, PrintStream out2)
	{
		super(out1);
		
		this.out = out2;
	}
	
	public MpmcsErrorStream(OutputStream arg0) {
		super(arg0);
		// TODO Auto-generated constructor stub
	}

	public MpmcsErrorStream(String arg0) throws FileNotFoundException {
		super(arg0);
		// TODO Auto-generated constructor stub
	}

	public MpmcsErrorStream(File arg0) throws FileNotFoundException {
		super(arg0);
		// TODO Auto-generated constructor stub
	}

	public MpmcsErrorStream(OutputStream arg0, boolean arg1) {
		super(arg0, arg1);
		// TODO Auto-generated constructor stub
	}

	public MpmcsErrorStream(String arg0, String arg1)
			throws FileNotFoundException, UnsupportedEncodingException {
		super(arg0, arg1);
		// TODO Auto-generated constructor stub
	}

	public MpmcsErrorStream(File arg0, String arg1)
			throws FileNotFoundException, UnsupportedEncodingException {
		super(arg0, arg1);
		// TODO Auto-generated constructor stub
	}

	public MpmcsErrorStream(OutputStream arg0, boolean arg1, String arg2)
			throws UnsupportedEncodingException {
		super(arg0, arg1, arg2);
		// TODO Auto-generated constructor stub
	}
	
	public void write(byte buf[], int off, int len) 
	{
        try 
        {
            super.write(buf, off, len);
            out.write(buf, off, len);
        } 
        catch (Exception e) 
        {
        }
    }
    public void flush() 
    {
        super.flush();
        out.flush();
    }

}
