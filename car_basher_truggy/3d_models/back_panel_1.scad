top_hole_center = 45 - 15 - 3/2;

panel_height = 22 + 4 + 10;
panel_width = 22;
panel_thickness = 4;

ard_base_z_offset = 10;

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

module nose( x, y, z, extrude, d){
    position = [ x, y, z ];
    turn_angle = 180+90; // extrude only takes positive height values
    translate( position )
    rotate (a=[ 0, turn_angle, 0 ])
    linear_extrude ( height = extrude+x, center = false)
    circle($fn=36, d=d, center = true);
}

module screw_hole1( x, y, thickness, d){
    linear_extrude (height=10*thickness);    
    position = [ 0, 0, y ];
    turn_angle = 90;
    translate( position ) rotate (a=[ 0, turn_angle, 0 ]) color("blue") circle(d=d, center=true);
}

module screw_hole( x, y, z, d, angle = 90){
    position = [ x, y, z ];
    turn_angle = 180+angle; // extrude only takes positive height values
    translate( position )
    rotate (a=[ 0, turn_angle, 0 ])
    linear_extrude ( height = 100, center = true)
    circle($fn=36, d=d, center = true);
}

module solids(){
    union() {
        back_panel();
        back_panel_top();
        arduino_bed();
        noses();
        controller_bed();
        
        //draw_ard_outline();
    }
}

module all_holes(){
        union(){
            panel_screw_holes();
            ard_screw_holes();
        }
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

module back_panel() {    
    cube_size = [panel_thickness, panel_width, panel_height];
    position = [ 0, - panel_width/2, 0 ];    
    color("yellow") translate( position ) cube(cube_size);
}

module back_panel_top() {
    h = 16;
    top_expansion = 30;
    w = 44;
    cube_size = [panel_thickness, w, h + top_expansion];
    position = [ 0, - w/2, panel_height-h ];    
    color("yellow") translate( position ) cube(cube_size);
}

module panel_screw_holes(){
    // middle
    screw_hole( x = 0, y = 0, z = top_hole_center-18, d = 3);
    // top left
    screw_hole( x = 0, y = -16.5, z = top_hole_center, d = 3);
    // top rightt
    screw_hole( x = 0, y = +16.5, z = top_hole_center, d = 3);
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

module noses(){
    extrude = 2;
    color("blue")
    nose( x = panel_thickness, y = 0, z = top_hole_center, extrude = extrude, d = 3);
    color("blue")
    nose( x = panel_thickness, y = 0, z = top_hole_center-6, extrude = extrude, d = 3);
    color("blue")
    nose( x = panel_thickness, y = 0, z = top_hole_center-12, extrude = extrude, d = 3);
}


