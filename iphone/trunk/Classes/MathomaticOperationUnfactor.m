//
//  MathomaticOperationSolve.m
//  mathomatic
//
//  Created by Ben Gotow on 11/21/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "MathomaticOperationUnfactor.h"


@implementation MathomaticOperationUnfactor

- (id)init
{
    if (self = [super init]){
        self.name = @"Unfactor";
    }
    return self;
}

// If a string is returned, it is displayed as an error
- (NSString*)perform
{
    [self performMathomaticSetup];
    [self evaluateMathomaticString: @"clear all"];
    [self evaluateExpression: [controller lastExpression]];
    
    NSString * result = [self evaluateMathomaticString: @"unfactor fully"];
    if ([result isEqualToString: @"Equation space is empty."])
        return @"The expression could not be unfactored.";
        
    [self addInputMathomaticString: result];
    [controller release];
    controller = nil;
    return nil;
}

// If a string is returned, it is displayed as an error
- (NSString*)prepareWithDelegate:(MathomaticViewController*)c
{
    controller = [c retain];
    [controller performCommand: self];
    return nil;
}

@end
