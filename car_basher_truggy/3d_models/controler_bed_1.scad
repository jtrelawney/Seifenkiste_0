//top_hole_center = 45 - 15 - 3/2;
//ard_base_z_offset = 10;

thickness = 3;  // how thick is controller bed

module controller_bed(position=[0,0,0]){
    screw_thick = 2.1;
    
    dx = 0;
    
    frame = 2;  // arround the pcb
    w = 59;     // width of pcb measured
    l = 31;     // length of pcb measure
    width = w+2*frame;
    length = l+2*frame;
    cube_inside = [ length, width, thickness];
    
    color("yellow")
    translate(position)
    difference(){
        cube(cube_inside);
        {
            
        // looking at pcb from top with arduino cable on upper right and leds on lower left
            
        // bottom holes are all 1mm from boarder
        hole_dist_bottom = 1;
        
        // lower right corner of pcb
        dx = frame;
        dy = frame;
            
        // bottom right hole is 1mm from each border
        x1 = dx+1+screw_thick/2;
        y1 = dy+1+screw_thick/2;
        screw_hole( x = x1, y = y1, z = 0, d = screw_thick, angle = 0);

        // bottom middle hole is 1mm from bottom and the outer edges of the screwholes are 26mm appart
        x2 = x1;
        // use measured distance, however the edges are each 1/2 of the screwthickness too far
        // hence deduct 1/2 for each
        y2 = y1+26-screw_thick; 
        screw_hole( x = x2, y = y2, z = 0, d = screw_thick, angle = 0);
        
        // bottom left hole is 1mm from bottom and the outer edges of the screwholes are 33mm appart
        x3 = x1;
        // use measured distance, however the edges are each 1/2 of the screwthickness too far
        // hence deduct 1/2 for each
        y3 = y2+33-screw_thick;
        screw_hole( x = x3, y = y3, z = 0, d = screw_thick, angle = 0);

        // upper right hole is 1mm from each border
        x4 = dx+l-1-screw_thick/2;
        y4 = dy+1+screw_thick/2;
        screw_hole( x = x4, y = y4, z = 0, d = screw_thick, angle = 0);

        // bottom middle hole is 1mm from top and the outer edges of the screwholes are 26mm appart
        x5 = x4;
        y5 = y4 + 26 - screw_thick;
        screw_hole( x = x5, y = y5, z = 0, d = screw_thick, angle = 0);
        } // holes
    }//difference
}

module screw_hole( x, y, z, d, angle = 90){
    position = [ x, y, z ];
    turn_angle = 180+angle; // extrude only takes positive height values
    translate( position )
    rotate (a=[ 0, turn_angle, 0 ])
    linear_extrude ( height = 100, center = true)
    circle($fn=36, d=d);
}

module myM3screw(position = [0,0,0]){
    screw_d = 3.2;
    top_d = 6;

    screw_h = 20;
    top_h = 35;
    
    z=-screw_h;
    pos_screw = [0,0,z];
    pos_top = [0,0,z+screw_h];    
    
    color("black")
    translate(position)
    union(){
    translate(pos_screw)
    linear_extrude ( height = screw_h, center = false)
    circle($fn=36, d=screw_d);
    
    translate(pos_top)
    linear_extrude ( height = top_h, center = false)
    circle($fn=36, d=top_d);
    }
}
//myM3screw([3,3,30]);

    
// size of shoe
shoe_width = 6.5;
shoe_length = 10;
shoe_height = 4+5; // groove = 5mm, base is 4mm
bridge_height = 25-10; // how high is the controller positioned

module one_shoe(position=[0,0,0]){


    cube_size = [shoe_width, shoe_length, shoe_height];
  
    // position of screwhole - left is 2mm, then hol
    x = 2 + 3/2;//width/2;
    y = shoe_length/2;    
    // how thick is area where screw holds down the shoe
    offset_screw_head = 4;
    
    translate(position)
    difference(){
        color("yellow",0.35)
        cube(cube_size);
        myM3screw([x,y,offset_screw_head]);
    }
}

module bridge(w,d,h, position=[0,0,0]){
    cube_size = [w,d,h];
    color("green")
    translate(position)
    cube(cube_size);
}

module solids(){
    
    groove_height = 2;  // height of the groove between the screws
    x_distance = 14;   // distance between the shoes, facing inner edges
    bridge_shoe_overlap = 2;        // how much does the bridge reach into the shoes
    bridge_thickness = 10;
    union() {
        x_bed = -(31/2 + 2) - shoe_length - 3.5;
        y_bed = -bridge_shoe_overlap/2-1;
        z_pos = bridge_height-thickness;
        rotate([0,0,180])
        controller_bed(position=[x_bed,y_bed,z_pos]);
        pos1=[0,0,0];
        one_shoe(pos1);
        // rotate because the screw hole is not centered, then move stuff over as per size
        // after first print the distance between the shoes was to short by ~6-7mm
        // when I added an additional shoe_width to the distance it worked, not sure why
        pos2=[-2*shoe_width-x_distance,-shoe_length,0];
        rotate([0,0,180]) one_shoe(pos2);

        top_height = 13;    // 13 is max or the  thin bridge starts moving down
        // this is the bulk part connected to the pillars
        // as above : added one more shoe_width the width
        main_x = x_distance+2*shoe_width;
        main_y = bridge_thickness;
        main_z = bridge_height - groove_height - top_height;
        main_bridge_pos = [0,-bridge_thickness+bridge_shoe_overlap,groove_height];
        bridge(main_x,main_y,main_z,main_bridge_pos);
        
        // this is the thinner part, a little thinner so that screws holes for the controller fit
        thinner = 5;
        thin_x = x_distance+2*shoe_width - thinner;
        thin_y = bridge_thickness;
        thin_z = top_height;
        thin_bridge_pos = [thinner/2,-bridge_thickness+bridge_shoe_overlap,groove_height+main_z];
        bridge(thin_x, thin_y, thin_z, thin_bridge_pos);
        
         // this is the connection between the shoes
        conn_x = x_distance;
        conn_y = bridge_thickness;
        conn_z = bridge_height - groove_height;
        conn_bridge_pos = [shoe_width,0,groove_height];
        bridge(conn_x, conn_y, conn_z, conn_bridge_pos);
        
    }
}

solids();
