import java.util.*;

public class Problem1{
    public static void main(String args[]){
        Scanner input = new Scanner(System.in);  // Create a Scanner object
        int times = input.nextInt();
        String inputSample;
        String result;
        Map<Character, Integer> duplicate;
        while (times-->0){
            inputSample = input.nextLine();
            duplicate = new HashMap<>();
            result = "";
            for(char character:inputSample.toCharArray()){
                if (duplicate.containsKey(character))continue;
                result+=character;
                duplicate.put(character, 1);
            }
            System.out.println(inputSample.length()+" "+result.length());
        }
        input.close();
    }
}