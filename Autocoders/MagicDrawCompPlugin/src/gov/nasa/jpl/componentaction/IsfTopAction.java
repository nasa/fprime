
package gov.nasa.jpl.componentaction;


import java.awt.event.ActionEvent;
import java.io.File;
import java.io.IOException;
import com.nomagic.magicdraw.actions.MDAction;
import com.nomagic.magicdraw.core.Application;
import com.nomagic.magicdraw.core.Project;

/**
 * 
 * Used to generate only the topology XML diagrams.
 *
 */
public class IsfTopAction extends MDAction {
	File pluginDir;

	/**
	 * 
	 */
	private static final long serialVersionUID = -6790954285526957354L;
	public IsfTopAction(String id, String name, File pluginDir)
    {
        super(id, name, null, null);
        this.pluginDir = pluginDir;
    }
	
	/**
	 * Runs the ProcessISFProject.process function.
	 * 
     * @see java.awt.event.ActionListener#actionPerformed(java.awt.event.ActionEvent)
     */
    public void actionPerformed(ActionEvent e)
    {
		Project project = Application.getInstance().getProject();
		try {
			ProcessISFTopology.process(project, pluginDir);
			System.out.println("** Completed Topology processing");
			System.out.println("*** Processing Complete!");
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
			System.out.println("**** " + e1.getMessage());
			System.err.flush();
		}
		//System.err.flush();
  
    }
}
