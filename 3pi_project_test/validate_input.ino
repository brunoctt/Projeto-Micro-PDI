int validate_input(bool validating_location, int total_vert, int aux_vertices){
  while (true){
    if (Serial.available()){
      String r = Serial.readString();
      r.trim(); // remove leading trailing white space e.g. CR etc
      int result = r.toInt();
      if (r != (String(result))) {
        Serial.print(r); Serial.println(" is not an integer");
        if (validating_location)
          Serial.println("Enter robot location: ");
        else
          Serial.println("Enter destination:");
      } else if (robot_location == result && !validating_location){
        Serial.print("Robot already at node "); Serial.println(result);
        Serial.println("Enter destination:");
      } else if (result < 0 || result  >= total_vert - aux_vertices){
        Serial.print("Invalid node number: ");
        Serial.println(result);
        if (validating_location)
          Serial.println("Enter robot location: ");
        else
          Serial.println("Enter destination:");
      } else{
        Serial.println(result);
        return result;
      }
  }}}  
