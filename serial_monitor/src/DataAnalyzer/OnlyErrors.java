package DataAnalyzer;
import java.io.BufferedReader;
import java.io.File;  // Import the File class
import java.io.FileNotFoundException;  // Import this class to handle errors
import java.io.FileReader;
import java.util.ArrayList;
import java.util.Scanner; // Import the Scanner class to read text files
import java.io.FileWriter;   // Import the FileWriter class
import java.io.IOException;

public class OnlyErrors {

	public static void main(String[] args) throws IOException {
		ArrayList<String> toPrint = new ArrayList<String>(); 
		boolean skip = false;
		boolean keepSkip = false;
		int num = 0;
		
	    String reference = "Jeff Bezos is stepping down as chief executive of Amazon.";
		try {
			
			File file = new File("C:\\Users\\gameb\\OneDrive\\Desktop\\Docs\\Science Fair\\2021\\Testing Data\\750_bezos_raw.txt");
			File newFile = new File("C:\\Users\\gameb\\OneDrive\\Desktop\\Docs\\Science Fair\\2021\\Testing Data\\750_bezos_reduced.txt");
			FileWriter myWriter = new FileWriter(newFile);
			BufferedReader scanner = new BufferedReader(new FileReader(file));
			String data;
			
			while((data = scanner.readLine()) != null) {
				System.out.println(++num);
		        toPrint.add(data);
		        
		        // resets per entry
		        if (data.length() >= 5 && data.substring(0,5).equals("-----")) {
		        	skip = true;
		        	for (int i = 0; i < toPrint.size(); i++) {
		        		// determines whether data has error
		        		String test = toPrint.get(i);
		        		if (test.length() >= 18 && test.substring(0,18).equals("Message Received: ")) {
		        			
		        			String str = test.substring(18);
		        			System.out.println("Str: " + str);
		        			if (str.length() < reference.length()) {
		        				// comment out this if statement and uncomment if below to create edited file
		        				skip = true;
		        				keepSkip = true;
		        			}
		        		}
		        		else if (test.length() > 19 && test.substring(0,19).equals("Connection Severed.") || test.equals("Message Received: ")) {
	        				skip = true;
	        				keepSkip = true;
		        		}
		        		
		        		else if (test.length() >= 1 && test.substring(0,1).equals("L")) {
		        			if (!keepSkip) skip = false;
		        		}
		        	}
		        	//if (!keepSkip) skip = false;
		        	if (!skip) {
		        		for (int i = 0; i < toPrint.size(); i++) {
		        			System.out.println("----> " + toPrint.get(i));
		        			
		        			if (!toPrint.get(i).equals("")) myWriter.write(toPrint.get(i) + "\n");// toPrint.get(i) + "\n");
		        			}
		        	}
		        	else {
		        		for (int i = 0; i < toPrint.size(); i++) {
		        			System.out.println(toPrint.get(i));
		        			}
		        	}
		        	
		        	// resets list
		        	toPrint = new ArrayList<String>(); 
		        	keepSkip = false;
		        	skip = false;
		        }
		        
			}
			myWriter.close();
			scanner.close();
			System.out.println("Successfully wrote to the file.");
			
			
		} catch (FileNotFoundException e) {
		      System.out.println("File not found.");
		      e.printStackTrace();
		}
		
		
	}
}
