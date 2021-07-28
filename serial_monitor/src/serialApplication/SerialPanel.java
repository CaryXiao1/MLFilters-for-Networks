package serialApplication;
import java.awt.Font;
import java.util.ArrayList;
import java.util.List;

import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.border.TitledBorder;

import DataAnalyzer.GenerateData;
import weka.core.Attribute;
import weka.core.DenseInstance;
import weka.core.Instance;

/** This class was created by Cary Xiao on January 16th, 2021.
 *  Functions as panel for received information. Also contains
 *  code for ML algorithm.
 */
public class SerialPanel extends JPanel {
	private static final long serialVersionUID = 1L;
	
	private final String STARTING_STRING = new String("String Transfer with Stochastic Gradient Descent Error Reduction.\nThis program can only transfer up to 140 characters at a time, so please shorten your messages.\nHappy messaging!\n______________________________________________________________________________");
	
	private JTextArea serialMonitor;
	private JScrollPane scroll;
	private MLCorrector corrector;
	
	// Stores error-flagged bits given from Arduino
	private int error[] = new int[8];
	private int errorSub[] = new int[8];
	private int e = 0; // index for both errors
	
	public SerialPanel(Font font) {
		
		serialMonitor = new JTextArea(STARTING_STRING, 37, 100);
		serialMonitor.setEnabled(true);
		serialMonitor.setFont(font);
		scroll = new JScrollPane(serialMonitor, JScrollPane.VERTICAL_SCROLLBAR_ALWAYS, JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
		
		this.setBorder(new TitledBorder(""));
		this.add(scroll);
		
		try {
			corrector = new MLCorrector();
		} catch (Exception e) {
			// TODO Auto-generated catch block
			//e.printStackTrace();
			addData("Failed to Initialize ML Corrector.");
		}
	}
	
	/* Adds new information to data, stores locations marked for error,
	 * runs ML model on error
	 */
	public void addData(String data) {
		// Stores locations marked for error.
		if (data.length() > 19 && data.substring(0, 20).equals("Locations of Error: ")) {
			String test = data.substring(20);
			int a = 0, b = 1; // Used to parse locations from string
				
			if (e > 0) {
				// resets marked error locations if not already reset
				e = 0;
				error = new int[8];
				errorSub = new int[8];
			}
			

			if (test.charAt(a) == '{') { 
				
				// parses and adds all non-trivial numbers (!= -1)
				while (b < test.length() - 1) {
					char bChar = test.charAt(b);
					if (bChar != ',' && bChar != ';') {
						b++;
					}
					else {
						String str = test.substring(a + 1, b);
						System.out.println("str: " + str);
						int set = Integer.parseInt(str);
						
						
						if (set >= 0) {
							if (bChar == ',') error[e] = set;
							if (bChar == ';') errorSub[e++] = set;
						}
						a = b;
						b++;
					}
				}
				for (int i = 0; i < error.length; i++) {
				System.out.println("error[" + i + "]: " + error[i]);
				System.out.println("errorSub[" + i + "]: " + errorSub[i]);
				}
			}
		}
		
		
		// runs ML-Model on data
		if (data.length() >= 19 && data.substring(0, 19).equals("ML-Corrected Data: ")) {
			String sentence; 
			String result = null;
			
			for (int i = 0; i < error.length; i++) {
				sentence = data.substring(19);
				if (error[i] >= 0 && error[i] < sentence.length() && errorSub[i] > 0) {
					
					byte alternative = 0;
					alternative = (byte) (((byte) sentence.charAt(error[i])) + Math.pow(2, 7 - errorSub[i])); 
					
					String test = GenerateData.sentenceToWord(sentence, error[i]);
					
					if (corrector.addToArff(test, (char) alternative)) {
						int bit = (int) corrector.test();
						System.out.println(bit);
						if (bit == 1) {
							System.out.println("Got into 1");
							result = data.substring(0, 19 + error[i]) + (char) alternative;
							if (result.length() < data.length()) result = result.concat(data.substring(19 + error[i] + 1, data.length()));
							System.out.println(result);
							data = result;
						}
						else if (bit == 0){
							System.out.println("Got into 0");
							result = data;

						}
					}
					if (result == null) {
						serialMonitor.append("\nSomething wrong happened.");
						break;
					}

				}
			}
			
			serialMonitor.append("\n" + data);
			
			// resets marked error locations
			e = 0;
			error = new int[8];
			errorSub = new int[8];
		}
		else serialMonitor.append("\n" + data);
		
		
		
	}
}
