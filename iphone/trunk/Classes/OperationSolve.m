//
//  OperationIntegrate.m
//  mathomatic
//
//  Created by Ben Gotow on 10/25/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "OperationSolve.h"
#import "MathomaticViewController.h"
#import "EquationSelectController.h"

@implementation OperationSolve

- (void)run
{
    if ((entry != nil) && ([entry isEqualToString:@""] == NO)){
        MathCommand * c = [[MathCommand alloc] init];
        [c addInput: entry];
        [delegate performCommand: c];
        [c release];
    }
    
    EquationSelectController * controller = [[EquationSelectController alloc] initWithEquations: delegate.commandStack andOperation: self];
    if (controller) [delegate presentModalViewController:controller animated:YES];
}

- (void)equationsSelected:(NSArray*)activeEquations solveVariables:(NSArray*)activeVariables fromVariables:(NSArray*)allVariables
{
    for (NSString * solveFor in activeVariables)
    {
        MathCommand * c = [[MathCommand alloc] init];
        [c addInput: @"clear all"];
        
        for (MathCommand * eq in activeEquations)
            [c addInput: [eq output]];
        
        for (NSString * variable in allVariables)
            if (![solveFor isEqualToString: variable])
                [c addInput: [NSString stringWithFormat: @"eliminate %@", variable]];
        
        [c addInput: [NSString stringWithFormat: @"solve %@", solveFor]];
        [delegate performCommand: c];
        [c release];
            
    }
    [self release];
}

@end
