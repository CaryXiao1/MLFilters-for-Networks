package DataAnalyzer;
import java.io.File;  // Import the File class
import java.io.FileNotFoundException;  // Import this class to handle errors
import java.io.FileWriter;
import java.util.ArrayList;
import java.util.Scanner; // Import the Scanner class to read text files
import java.io.IOException;

public class AnalyzeML {

	public static void main(String[] args) throws IOException {
		int netAccuracy = 0;
		int totalErrors = 0;
		ArrayList<String> toPrint = new ArrayList<String>(); 
		int[] error = new int[8];
		int e = 0;
		String reference = "Jeff Bezos is stepping down as chief executive of Amazon.";
		
		try {
			
			File file = new File("C:\\Users\\gameb\\OneDrive\\Desktop\\Docs\\Science Fair\\2021\\Testing Data\\500_bezos_onlyML_new.txt");
			//File newFile = new File("C:\\Users\\gameb\\OneDrive\\Desktop\\Docs\\Science Fair\\2021\\Testing Data\\Jeff Bezos\\500_bezos_resultsML.txt");
			Scanner scanner = new Scanner(file);
			//FileWriter myWriter = new FileWriter(newFile);
			
			
			while(scanner.hasNextLine()) {
		        String data = scanner.nextLine();
		        toPrint.add(data);
		        
		        // resets per entry
		        if (data.length() >= 5 && data.substring(0,5).equals("-----")) {
		        	for (int i = 0; i < toPrint.size(); i++) {
		        		String test = toPrint.get(i);
		        		if (test.length() > 19 && test.substring(0,20).equals("Locations of Error: ")) {
		        			
		        			// Parses error location string for locations
		        			String testData = test.substring(20);
		        			int a = 0, b = 1;	        			
		        			if (testData.charAt(a) == '{') { 
		        				while (b < testData.length() - 1) {
		        					char bChar = testData.charAt(b);
		        					if (bChar != ',' && bChar != ';') {
		        						b++;
		        					}
		        					else {
		        						String str = testData.substring(a + 1, b);
		        						int set = Integer.parseInt(str);
		        				
		        						if (set >= 0 && bChar == ',') {
		        							error[e] = set;
		        							e++;
		        						}
		        						a = b;
		        						b++;
		        					}
		        				}
		        			}
		        			
		        			
		        		}
		        		
		        		else if (test.length() >= 19 && test.substring(0, 19).equals("ML-Corrected Data: ")) {
		        			
		        			// Tests specific point based off reference
		        			String testData = test.substring(19);
		        			for (int j = 0; j < error.length && error[j] != 0; j++) {
		        			if (error[j] < reference.length() ) {
		        				totalErrors++;
		        				System.out.println(testData.charAt(error[j]) + ", " + reference.charAt(error[j]));
		        				//myWriter.write(testData.charAt(error[j]) + ", " + reference.charAt(error[j]) + "\n");
		        				if (testData.charAt(error[j]) == reference.charAt(error[j])) netAccuracy++;
			        			else netAccuracy--;
		        			}
		        			}
		        		}
		        		
		        	}
		        	

		        		for (int i = 0; i < toPrint.size(); i++) {
		        			System.out.println(toPrint.get(i));
		        			//myWriter.write(toPrint.get(i) + "\n");
		        			}
		        	
		        	
		        	// resets list, error Array
		        	error = new int[8];
		        	toPrint = new ArrayList<String>(); 
		        	e = 0;
		        	System.out.println("Net ML Accuracy: " + netAccuracy);
		        	//myWriter.write("Net ML Accuracy: " + netAccuracy + "\n");
		        	System.out.println("Total Valid Errors: " + totalErrors);
		        	//myWriter.write("Total Valid Errors: " + totalErrors + "\n");
		        }
			}
			//myWriter.close();
			
		} catch (FileNotFoundException exception) {
		      System.out.println("File not found.");
		      exception.printStackTrace();
		}
		
		
	}
}
