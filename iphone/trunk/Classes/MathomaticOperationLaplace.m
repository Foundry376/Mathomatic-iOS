//
//  MathomaticOperationSolve.m
//  mathomatic
//
//  Created by Ben Gotow on 11/21/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "MathomaticOperationLaplace.h"


@implementation MathomaticOperationLaplace

- (id)init
{
    if (self = [super init]){
        self.name = @"Laplace";
    }
    return self;
}

// If a string is returned, it is displayed as an error
- (NSString*)perform
{
    if ([[controller lastExpression] isEquation]){
        UIAlertView * warning = [[UIAlertView alloc] initWithTitle:@"Note" message:@"The expression was an equation. Mathomatic only took the laplace transform of the right hand side." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [warning show];
        [warning release];
    }
    
    [self setName: [NSString stringWithFormat:@"Laplace (%@)", [variable uppercaseString]]];
    
    [self performMathomaticSetup];
    [self evaluateMathomaticString: @"clear all"];
    [self evaluateExpression: [controller lastExpression]];
    [controller release];
    controller = nil;
    
    NSString * result =  [self evaluateMathomaticString: [NSString stringWithFormat: @"laplace %@", variable]];
    if ([result isEqualToString:@"Laplace failed, not a polynomial."])
        return @"Laplace transform failed. The expression is not a polynomial.";
    else if ([result isEqualToString:@"No current equation or expression."])
        return @"Laplace transform failed. The expression is not a polynomial.";
    else {
        result = [result stringByReplacingOccurrencesOfString:@" " withString:@""];
        [self addInputMathomaticString: result];
    }
    return nil;
}

// If a string is returned, it is displayed as an error
- (NSString*)prepareWithDelegate:(MathomaticViewController*)c
{
    controller = [c retain];
        
    UIActionSheet * sheet = [[UIActionSheet alloc] initWithTitle:@"For variable:" delegate:self cancelButtonTitle:@"Cancel" destructiveButtonTitle:nil otherButtonTitles: @"X",@"Y",@"Z",nil];    
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
