//
//  Mathomatic_Simulator_TestViewController.m
//  Mathomatic Simulator Test
//
//  Created by Ben Gotow on 11/19/08.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//

#import "Mathomatic_Simulator_TestViewController.h"
#import "mathomatic.h"

@implementation Mathomatic_Simulator_TestViewController



/*
// Override initWithNibName:bundle: to load the view using a nib file then perform additional customization that is not appropriate for viewDidLoad.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        // Custom initialization
    }
    return self;
}
*/

/*
// Implement loadView to create a view hierarchy programmatically.
- (void)loadView {
}
*/


// Implement viewDidLoad to do additional setup after loading the view.
- (void)viewDidLoad {
    [super viewDidLoad];

    matho_init();
    char * output;
    NSString * s;
    
    matho_process("x^2=4", &output);
    matho_process("solve x", &output);
    NSLog([NSString stringWithCString: output]);
    
    matho_process("x = 3*i#+3/4*y",&output);
    NSLog([NSString stringWithCString: output]);
    matho_process("y = i#*3",&output);
    NSLog([NSString stringWithCString: output]);
    matho_process("simplify sign",&output);
    NSLog([NSString stringWithCString: output]);
    
    matho_process("approximate",&output);
    NSLog([NSString stringWithCString: output]);
}


- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
    // Release anything that's not essential, such as cached data
}


- (void)dealloc {
    [super dealloc];
}

@end
