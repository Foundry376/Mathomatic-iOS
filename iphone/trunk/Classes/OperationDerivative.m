//
//  OperationIntegrate.m
//  mathomatic
//
//  Created by Ben Gotow on 10/25/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "OperationDerivative.h"
#import "mathomaticViewController.h"


@implementation OperationDerivative

- (void)run
{
    UIActionSheet * sheet = [[UIActionSheet alloc] initWithTitle:@"With respect to:" delegate:self cancelButtonTitle:@"Cancel" destructiveButtonTitle:nil otherButtonTitles:@"X",@"Y",@"Z",nil];
    [sheet showInView: [delegate view]];
    [sheet release];
}

- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
    NSString * variable = nil;
    
    if (buttonIndex == 0)
        variable = @"x";
    if (buttonIndex == 1)
        variable = @"y";
    if (buttonIndex == 2)
        variable = @"z";
    
    if (variable != nil){
        MathCommand * c = [[MathCommand alloc] init];
        [c addInput: entry];
        [c addInput: [NSString stringWithFormat: @"derivative %@", variable]];
        [delegate performCommand: c];
        [c release];
    }
    
    [self release];
}


@end
