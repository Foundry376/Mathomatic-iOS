//
//  MathomaticOperationSolve.m
//  mathomatic
//
//  Created by Ben Gotow on 11/21/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "MathomaticOperationFactor.h"


@implementation MathomaticOperationFactor

- (id)init
{
    if (self = [super init]){
        self.name = @"Factor";
    }
    return self;
}

- (NSString*)perform
{
    [self performMathomaticSetup];
    [self evaluateMathomaticString: @"clear all"];
    [self evaluateExpression: [controller lastExpression]];
    
    NSString * result = [self evaluateMathomaticString: @"factor"];
    if ([result isEqualToString:@"Not a valid integer or number too large to factor."])
        return @"The expression could not be factored.";
    if ([result isEqualToString: @"Equation space is empty."])
        return @"The expression could not be factored.";
    
    [self addInputMathomaticString: result];
    [controller release];
    controller = nil;
    return nil;
}

- (NSString*)prepareWithDelegate:(MathomaticViewController*)c
{
    controller = [c retain];
    [controller performCommand: self];
    return nil;
}

@end
