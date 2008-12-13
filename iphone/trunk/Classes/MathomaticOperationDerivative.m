//
//  MathomaticOperationSolve.m
//  mathomatic
//
//  Created by Ben Gotow on 11/21/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "MathomaticOperationDerivative.h"


@implementation MathomaticOperationDerivative

- (id)init
{
    if (self = [super init]){
        self.name = @"Derivative";
    }
    return self;
}

// If a string is returned, it is displayed as an error
- (NSString*)perform
{
    if (variable == nil){
        [self addInputMathomaticString: @"0"];
        [controller release];
        controller = nil;
        return nil;
    }
    
    BOOL wasEquation = [[controller lastExpression] isEquation];
    NSString * input = [[controller lastExpression] mathomaticText];
    
    if (!wasEquation){
        input = [NSString stringWithFormat: @"a=%@", input];
    }
        
    [self setName: [NSString stringWithFormat:@"Derivative (d/d%@)", variable]];
    [self performMathomaticSetup];
    [self evaluateMathomaticString: @"clear all"];
    [self evaluateMathomaticString: input];
    [controller release];
    controller = nil;
    
    NSString * result =  [self evaluateMathomaticString: [NSString stringWithFormat: @"derivative %@", variable]];
    
    if ([result isEqualToString: @"Current expression contains no variables."])
        return @"Try taking the derivative of each side of the equation by itself.";
    else if ([result isEqualToString: @"Variable not found; the derivative would be zero."])
        if (wasEquation)
            return @"Try taking the derivative of each side of the equation by itself.";
        else
            return @"The derivative would be zero.";
    else if (([result isEqualToString: @"Differentiation failed."]) || ([result isEqualToString: @"No current equation or expression."]))
        return @"The derivative could not be calculated.";
    else {
        result = [result stringByReplacingOccurrencesOfString:@" " withString:@""];
        if (wasEquation){
            UIAlertView * warning = [[UIAlertView alloc] initWithTitle:@"Note" message:@"The expression was an equation. Mathomatic only took the derivative of the right hand side." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
            [warning show];
            [warning release];
            [self addInputMathomaticString: result];
        } else
            [self addInput: [[MathomaticExpression expressionWithMathomaticText: result] rhs]];
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
    if ([vars count] > 1){
        // If there's more than one variable, display the sheet
        UIActionSheet * sheet = [[UIActionSheet alloc] initWithTitle:@"With respect to:" delegate:self cancelButtonTitle:nil destructiveButtonTitle:nil otherButtonTitles: nil];
        for (NSString * var in vars)
            [sheet addButtonWithTitle: [var uppercaseString]];
        [sheet addButtonWithTitle: @"Cancel"];
        [sheet setCancelButtonIndex: [sheet numberOfButtons]-1];
        [sheet showInView: [controller view]];
        [sheet release];
        
    } else if ([vars count] > 0){
        // if there's one variable, just use that one
        variable = [[vars lastObject] retain];
        [controller performCommand: self];
        
    } else {
        // if there are no variables, run anyway. We'll just set the output to 0.
        variable = nil;
        [controller performCommand: self];
    }
    return nil;
}

- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
    // If the user has selected a variable from the list, store their selection and 
    // tell our controller to perform us.
    if (buttonIndex != [actionSheet cancelButtonIndex]){
        variable = [[[actionSheet buttonTitleAtIndex: buttonIndex] lowercaseString] retain];
        [controller performCommand: self];
    } else {
        [controller release];
        controller = nil;
    }
}


@end
