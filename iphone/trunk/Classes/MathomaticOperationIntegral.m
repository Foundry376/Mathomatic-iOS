//
//  MathomaticOperationSolve.m
//  mathomatic
//
//  Created by Ben Gotow on 11/21/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "MathomaticOperationIntegral.h"


@implementation MathomaticOperationIntegral

- (id)init
{
    if (self = [super init]){
        self.name = @"Integrate";
    }
    return self;
}

// If a string is returned, it is displayed as an error
- (NSString*)perform
{
    BOOL wasEquation = [[controller lastExpression] isEquation];
    NSString * input = [[controller lastExpression] mathomaticText];
    
    if (!wasEquation){
        input = [NSString stringWithFormat:@"a=%@",input];
    }
    
    [self setName: [NSString stringWithFormat:@"Integrate (d%@)", variable]];
    
    [self performMathomaticSetup];
    [self evaluateMathomaticString: @"clear all"];
    [self evaluateMathomaticString: input];
    [controller release];
    controller = nil;
    
    NSString * result =  [self evaluateMathomaticString: [NSString stringWithFormat: @"integrate %@", variable]];
    if ([result isEqualToString:@"No current equation or expression."])
        return @"Could not integrate. The expression does not contain any variables.";
    else if ([result isEqualToString:@"Integration failed, not a polynomial."])
        return @"Could not integrate. The expression is not a polynomial.";
    else {
        if (wasEquation){
            UIAlertView * warning = [[UIAlertView alloc] initWithTitle:@"Note" message:@"The expression was an equation. Mathomatic only integrated the right hand side." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
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
    UIActionSheet * sheet = [[UIActionSheet alloc] initWithTitle:@"With respect to:" delegate:self cancelButtonTitle:@"Cancel" destructiveButtonTitle:nil otherButtonTitles: @"X",@"Y",@"Z",nil];
    [sheet showInView: [controller view]];
    [sheet release];
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
