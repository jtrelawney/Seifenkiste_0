color_panel="yellow";//"aqua";//"DarkKhaki"; //"Goldenrod"
color_h="yellow";//color="ForestGreen";
color_noses="red";//"lime";

panel_thickness = 5;

// major distances in center column
bottom_to_first_hole = 7;
bottom_to_top = 46;
top_to_first_hole = 15;
over_top_for_ard = 0;

top_hole_center = bottom_to_top - 15 - 3/2;

// target width of the different sections
bottom_width = 12;
middle_width = 44;
top_width = 44;

module screw_hole( x, y, z, d, angle = 90){
    position = [ x, y, z ];
    turn_angle = 180+angle; // extrude only takes positive height values
    translate( position )
    rotate (a=[ 0, turn_angle, 0 ])
    linear_extrude ( height = 100, center = true)
    circle($fn=36, d=d);
}

module back_panel(){

    cube_bottom = [bottom_to_first_hole, bottom_width, panel_thickness];
    pos1 = [ 0, - bottom_width/2, 0 ];   
    cube_middle = [bottom_to_top- bottom_to_first_hole-top_to_first_hole, middle_width, panel_thickness];
    pos2 = [ bottom_to_first_hole, - middle_width/2, 0 ];   
    cube_top = [top_to_first_hole+over_top_for_ard, top_width, panel_thickness];
    pos3 = [ bottom_to_top - top_to_first_hole, - top_width/2, 0 ];   
    
    color(color_panel)
    union(){
        translate( pos1 ) cube(cube_bottom);
        translate( pos2 ) cube(cube_middle);
        translate( pos3 ) cube(cube_top);
    }
}

// creates the 4 holes given the bottom center position
// with_outline = adds the arduino shape for testing - remove for printing
// make_holes = if true the holes are empty, if false the holes are shown
module ard_holes(position=[0,0,0], with_outline = false, make_holes = false,color=color_h){
    
    ard_width = 102;
    ard_length = 54;
    ard_pos_dist = 10;
    
    // using standard coord. system

    // relative hole distance with h1 lower left, h2 lower right, h3 upper left, h4 upper right
    dx_12 = 96.52 - 13.97;
    dx_34 = 50.8 + 24.1;
    dy_13 = 50.8 - 2.54;
    
    // lower left pos on arduino board
    dy = 2.54;
    dx = 14.0;

    // coordinates of point middle of board bottom
    x0 = 0;
    y0 = 0;
    
    x1 = dx;
    y1 = dy;
    
    x2 = x1 + dx_12;
    y2 = y1;
    
    x3 = x1;
    y3 = y1 + dy_13;

    x4 = x1 + dx_34;
    y4 = y3;
    
    // this moves the 2 lower holes a little bit - as per first test print
    correction1 = 1;
    correction2 = -1;
    
    // this turns the H so that the power is on the left
    angles = [180,0,0];
    
    if (make_holes == false){
        rotate(angles)
        translate(position)
        union(){
        
                    union(){
                        arm_width = 10;
                        arm1_dim = [ arm_width, dx_12+10, panel_thickness ];
                        x_off1 = -dy;
                        y_off1 = -ard_width+5-5;
                        color(color) translate([ x_off1, y_off1, 0])
                        cube(arm1_dim);
                        x_off2 = -dy+dy_13;
                        y_off2 = -ard_width+5-5;
                        color(color) translate([ x_off2, y_off2, 0])
                        cube(arm1_dim);
                        x_off3 = -dy;
                        y_off3 = -ard_width/2-arm_width/2;
                        arm2_dim = [ dy_13, arm_width, panel_thickness ];
                        color(color) translate([ x_off3, y_off3, 0])
                        cube(arm2_dim);
                    }
                    
                    if (with_outline == true){
                        cube_size = [ ard_length, ard_width, 1 ];
                        color("green",0.25) translate([0,-ard_width,ard_pos_dist])
                        cube(cube_size);
                    }

                   
                        z = position[2];    
                        rotate(a=[ 0, 0, -90 ])
                        union(){
                        screw_hole( x = x1+correction1, y = y1, z = z, d = 3, angle = 0);
                        screw_hole( x = x2+correction2, y = y2, z = z, d = 3, angle = 0);
                        screw_hole( x = x3, y = y3, z = z, d = 3, angle = 0);
                        screw_hole( x = x4, y = y4, z = z, d = 3, angle = 0);
                        
                    }
                }
            } else { // wie oben aber difference mit loechern und ischer keine outline
                rotate(angles)
                translate(position)
                difference() {
                        union(){
                            arm_width = 10;
                            arm1_dim = [ arm_width, dx_12+10, panel_thickness ];
                            x_off1 = -dy;
                            y_off1 = -ard_width+5-5;
                            color(color) translate([ x_off1, y_off1, 0])
                            cube(arm1_dim);
                            x_off2 = -dy+dy_13;
                            y_off2 = -ard_width+5-5;
                            color(color) translate([ x_off2, y_off2, 0])
                            cube(arm1_dim);
                            x_off3 = -dy;
                            y_off3 = -ard_width/2-arm_width/2;
                            arm2_dim = [ dy_13, arm_width, panel_thickness ];
                            color(color) translate([ x_off3, y_off3, 0])
                            cube(arm2_dim);
                        }
                        
                      
                        z = position[2];    
                        rotate(a=[ 0, 0, -90 ])
                        union(){
                        screw_hole( x = x1+correction1, y = y1, z = z, d = 3, angle = 0);
                        screw_hole( x = x2++correction2, y = y2, z = z, d = 3, angle = 0);
                        screw_hole( x = x3, y = y3, z = z, d = 3, angle = 0);
                        screw_hole( x = x4, y = y4, z = z, d = 3, angle = 0);
                        
                    }
                }
                
            }   
            
}

module outline_key_marks(){
    // line at top
    length = 1000;
    cube_size1 = [ 1, length, 10 ];
    color("black", 1.25) translate([bottom_to_top,-length/2,0])
    cube(cube_size1);
    // line the size of the screws at top top
    m5_screw = 5;
    cube_size2 = [ m5_screw, length, 10 ];
    color("black", 1.25) translate([bottom_to_top + 7 - m5_screw,-length/2,0])
    cube(cube_size2);
}

module myM3screw(position = [10,10,0]){
    d = 3;
    h = 10;
    td = 5.5;
    th = 1.5;
    
    translate(position)
    union(){
    pos1 = [0,0,h/2];    
    color("black")
    translate(pos1)
    linear_extrude ( height = h, center = true)
    circle($fn=36, d=d);
    
    post = [0,0,th/2];    
    color("black")
    translate(post)
    linear_extrude ( height = th, center = true)
    circle($fn=36, d=td);
    }
}
//myM3screw([3,3,30]);

module screw_hole1( x, y, z, d, angle = 90){
    position = [ x, y, z ];
    turn_angle = 180+angle; // extrude only takes positive height values
    translate( position )
    rotate (a=[ 0, turn_angle, 0 ])
    linear_extrude ( height = 100, center = true)
    circle($fn=36, d=d, center = true);
}

module nose( x, y, z, extrude, d){
    position = [ x, y, z ];
    turn_angle = 0; //180; // extrude only takes positive height values
    translate( position )
    rotate (a=[ 0, turn_angle, 0 ])
    linear_extrude ( height = extrude, center = false)
    circle($fn=36, d=d);
}

module noses(color=color_noses){
    extrude = 5;
    color(color)
    nose( x = top_hole_center, y = 0, z = panel_thickness, extrude = extrude, d = 3);
    color(color)
    nose( x = top_hole_center-6, y = 0, z = panel_thickness, extrude = extrude, d = 3);
    color(color)
    nose( x = top_hole_center-12, y = 0, z = panel_thickness, extrude = extrude, d = 3);
}

module panel_screw_holes(){
    // correct the top holes 1mm inward as per test print
    correction = 1;
    // middle at bottom
    myM3screw([ top_hole_center-18, 0, 0]);
    // top left and right
    myM3screw([ top_hole_center, -16.5+correction, 0]);
    myM3screw([ top_hole_center, 16.5-correction, 0]);
}


difference(){
    union(){
    back_panel();
    noses();
    //place arduino at bottom of top nose
    x_pos_ard = bottom_to_top - top_to_first_hole - 9;
    ard_holes([x_pos_ard,102/2,-panel_thickness],with_outline = false, make_holes = true);
    }
    panel_screw_holes();
}
//outline_key_marks();



/*

module back_panel_bottom() {
    cube_size = [panel_height, panel_width, panel_thickness];
    position = [ 0, - panel_width/2, 0 ];    
    color("yellow") translate( position ) cube(cube_size);
}

module back_panel_top() {
    h = 16;
    top_expansion = 30;
    w = 44;
    cube_size = [w, h + top_expansion,panel_thickness];
    position = [panel_height-h,- w/2, 0];    
    color("yellow") translate( position ) cube(cube_size);
}



panel_height = 22 + 4 + 10;
panel_width = 22;

// verify below



ard_base_z_offset = 10;

module back_panel_bottom() {    
    cube_size = [panel_height, panel_width, panel_thickness];
    position = [ 0, - panel_width/2, 0 ];    
    color("yellow") translate( position ) cube(cube_size);
}

module back_panel_top() {
    h = 16;
    top_expansion = 30;
    w = 44;
    cube_size = [w, h + top_expansion,panel_thickness];
    position = [panel_height-h,- w/2, 0];    
    color("yellow") translate( position ) cube(cube_size);
}





module solids(){
    union() {
        //back_panel_bottom();
        //back_panel_top();
        //arduino_bed();
        noses();
        //controller_bed();        
        //draw_ard_outline();
        
        //test display holes
        panel_screw_holes();
        
    }
}

module all_holes(){
        union(){
            //panel_screw_holes();
            //ard_screw_holes();
        }
}

// make a box where the antena module can rest in
module controller_bed(){
    // parameters from arduino bed to calculate the position of the edge and place controller there
    ard_d = 53;
    frame_d = 3;
    dx = ard_d + 2*frame_d + panel_thickness;
    
    thickness = 3;
    base_pos_z = top_hole_center + ard_base_z_offset;
    
    frame = 2;
    w = 58;
    h = 32;
    cube_inside = [ h, w+frame, thickness];
    
    position = [dx, - (w+frame)/2, base_pos_z ];
    
    difference(){
        
    color("yellow") translate( position ) cube(cube_inside);
    
    {
    // top left of controller
    dy = -w/2;
    
    x1 = dx+2;
    y1 = dy+53;
    screw_hole( x = x1, y = y1, z = top_hole_center, d = 3, angle = 0);

    x2 = dx+2;
    y2 = dy+38;
    screw_hole( x = x2, y = y2, z = top_hole_center, d = 3, angle = 0);
    
    x3 = dx+2;
    y3 = dy+44;
    screw_hole( x = x3, y = y3, z = top_hole_center, d = 3, angle = 0);

    x4 = dx+29;
    y4 = dy+6;
    screw_hole( x = x4, y = y4, z = top_hole_center, d = 3, angle = 0);

    x5 = dx+29;
    y5 = dy+29;
    screw_hole( x = x5, y = y5, z = top_hole_center, d = 3, angle = 0);

    x6 = dx+29;
    y6 = dy+56;
    screw_hole( x = x6, y = y6, z = top_hole_center, d = 3, angle = 0);
    } // holes
    }//difference
}









difference(){
    solids();
    all_holes();
}

module arduino_bed() {
    // ard size is size of arduino pcb, the frame is added to give some space to the panel
    ard_d = 53;
    frame_d = 3;
    d = ard_d + 2*frame_d + panel_thickness;
    
    ard_w = 108;    
    base_h = 3;
    
    base_pos_z = top_hole_center + ard_base_z_offset;
    
    cube_size = [d, ard_w, base_h];
    position = [ 0, - ard_w/2, base_pos_z ];    
    color("yellow") translate( position ) cube(cube_size);
}
module draw_ard_outline(){
    y = 102;
    x = 54;
    h = 1;
    offset_x =  panel_thickness + 2;
    center_y = y/2;
    base_pos_z = top_hole_center - ard_base_z_offset + 22;
    cube_size = [x, y, h];
    position = [ offset_x, -center_y, base_pos_z + 2 ];
    color("green",0.25) translate(position) cube(cube_size);
}

module ard_screw_holes(){
    
    y = 102;
    x = 54;
    h = 2;
    offset_x =  panel_thickness + 2;
    center_y = y/2;
    base_pos_z = top_hole_center - 10 + 2;
    cube_size = [x, y, h];
    position = [ offset_x, -center_y, base_pos_z ];
    
    // taken from board_outline()
    upper_left_x = panel_thickness + 2;
    upper_left_y = -102/2;

    // top left board position
    //screw_hole( x = upper_left_x, y = upper_left_y, z = top_hole_center, d = 0.3, angle = 0);
    
    // top left hole
    x1 = upper_left_x + 53.3 - (2.5 + 5.1 + 27.9 + 15.2);
    y1 = upper_left_y + 14 + 1.3;
    screw_hole( x = x1, y = y1, z = top_hole_center, d = 3, angle = 0);

    // top left hole
    x2 = x1;
    y2 = y1 + 50.8+24.1;
    screw_hole( x = x2, y = y2, z = top_hole_center, d = 3, angle = 0);
    
    // bottom left hole
    x3 = upper_left_x + 53.3 - 2.5;
    y3 = y1 - 1.3;
    screw_hole( x = x3, y = y3, z = top_hole_center, d = 3, angle = 0);
    
    // bottom right hole
    x4 = x3;
    y4 = upper_left_y + 96.52;
    screw_hole( x = x4, y = y4, z = top_hole_center, d = 3, angle = 0);
}

*/
