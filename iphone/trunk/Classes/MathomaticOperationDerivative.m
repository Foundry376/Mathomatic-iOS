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
        
    [self performMathomaticSetup];
    [self setName: [NSString stringWithFormat:@"Derivative (d/d%@)", variable]];
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

- (NSString*)prepareWithDelegate:(MathomaticViewController*)c
{
    controller = [c retain];
    
    NSArray * vars = [[c lastExpression] equationVariables];
    if ([vars count] > 1){
        UIActionSheet * sheet = [[UIActionSheet alloc] initWithTitle:@"With respect to:" delegate:self cancelButtonTitle:nil destructiveButtonTitle:nil otherButtonTitles: nil];
        for (NSString * var in vars)
            [sheet addButtonWithTitle: [var uppercaseString]];
        [sheet addButtonWithTitle: @"Cancel"];
        [sheet setCancelButtonIndex: [sheet numberOfButtons]-1];
        [sheet showInView: [controller view]];
        [sheet release];
        
    } else if ([vars count] > 0){
        variable = [[vars lastObject] retain];
        [controller performCommand: self];
        
    } else {
        variable = nil;
        [controller performCommand: self];
    }
    return nil;
}

- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
    if (buttonIndex != [actionSheet cancelButtonIndex]){
        variable = [[[actionSheet buttonTitleAtIndex: buttonIndex] lowercaseString] retain];
        [controller performCommand: self];
    } else {
        [controller release];
        controller = nil;
    }
}


@end
