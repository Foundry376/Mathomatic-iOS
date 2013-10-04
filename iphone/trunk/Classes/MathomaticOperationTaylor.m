//
//  MathomaticOperationSolve.m
//  mathomatic
//
//  Created by Ben Gotow on 11/21/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "MathomaticOperationTaylor.h"
#import "TaylorSelectController.h"


@implementation MathomaticOperationTaylor

- (id)init
{
    if (self = [super init]){
        self.name = @"Taylor Expansion";
    }
    return self;
}

// If a string is returned, it is displayed as an error
- (NSString*)perform
{
    BOOL wasEquation = [[controller lastExpression] isEquation];
    NSString * input = [[controller lastExpression] mathomaticText];
    [controller release];
    controller = nil;
        
    [self setName: [NSString stringWithFormat:@"Taylor Expansion (%@=%@, Order=%d)", [variable uppercaseString], [[NSNumber numberWithInteger: point] stringValue], order]];
    [self performMathomaticSetup];
    [self evaluateMathomaticString: @"clear all"];
    [self evaluateMathomaticString: input];
    
    NSString * result = [self evaluateMathomaticString: [NSString stringWithFormat: @"taylor %@ %d %f", variable, order, point]];
    
    if ([result isEqualToString: @"Current expression contains no variables."])
        return @"Try taking the taylor expansion of each side of the equation by itself.";
    else {
        if (wasEquation){
            UIAlertView * warning = [[UIAlertView alloc] initWithTitle:@"Note" message:@"The expression was an equation. Mathomatic only performed a taylor expansion of the right hand side." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
            [warning show];
            [warning release];
        }
        result = [result stringByReplacingOccurrencesOfString:@" " withString:@""];
        [self addInputMathomaticString: result];
    }
    return nil;
}

// If a string is returned, it is displayed as an error
- (NSString*)prepareWithDelegate:(MathomaticViewController*)c
{
    controller = [c retain];
    
    // determine which variables are in the current expression and build a sheet.
    // Unlike the integrate and laplace transform commands, you can't take the derivative
    // with respect to a variable that doesn't exist!
    NSArray * vars = [[c lastExpression] equationVariables];
    if ([vars count] > 0){
        TaylorSelectController * select = [[TaylorSelectController alloc] initWithNibName:@"TaylorSelectController" bundle:[NSBundle mainBundle]];
        [c presentViewController:select animated:YES completion:NULL];
        [select setOperation: self];
        [select setAvailableVariables: vars];
        [select release];
    } else {
        return @"Please enter an expression with at least one variable to perform a taylor expansion.";
    }
    return nil;
}

- (void)parametersSelected:(NSString*)var point:(float)p order:(int)o
{
    variable = [var retain];
    point = p;
    order = o;
    [controller performCommand: self];
}

@end
