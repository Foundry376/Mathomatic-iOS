//
//  MathomaticOperationSolve.m
//  mathomatic
//
//  Created by Ben Gotow on 11/21/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "MathomaticOperationSolve.h"

@implementation MathomaticOperationSolve

- (id)init
{
    if (self = [super init]){
        self.name = @"Solving System of Equations";
    }
    return self;
}

// If a string is returned, it is displayed as an error
- (NSString*)perform
{
    NSMutableArray * variables = [NSMutableArray array];
    
    [self performMathomaticSetup];
    [self evaluateMathomaticString: @"clear all"];
    
    for (MathomaticExpression * i in inputs){
        [self evaluateExpression: i];
        for (NSString * var in [i equationVariables])
            if (![variables containsObject: var])
                [variables addObject: var];
    }
    
    if ([inputs count] > 1){
        for (NSString * var in variables){
            if (![variable isEqualToString: var]){
                NSString * result = [self evaluateMathomaticString: [NSString stringWithFormat: @"eliminate %@", var]];
                
                if ([result isEqualToString: @"No substitutions made."] != YES)
                    [self addStepMathomaticString:result withName: [NSString stringWithFormat: @"Eliminating %@", [var uppercaseString]]];
            }
        }
    }
    
    NSString * result = [self evaluateMathomaticString: [NSString stringWithFormat: @"solve %@", variable]];
    if (result == nil)
        return @"The system could not be solved.";
    
    [self addStepMathomaticString: result withName:[NSString stringWithFormat: @"Solving for %@", [variable uppercaseString]]];
     
    NSString * previous = nil;
    NSString * possible = nil;
    
    if ([result rangeOfString: @"sign"].location != NSNotFound){
        [self evaluateMathomaticString: @"simplify sign"];
        possible = [self evaluateMathomaticString: @"#+1"];
    }
    
    while (true){
        possible = [self evaluateMathomaticString: @"approximate"];
        if ((![possible isEqualToString: previous]) && (![possible isEqualToString:@"Equation space is empty."]) && (![possible isEqualToString: result])){
            [self addStepMathomaticString: possible withName:@"Solutions"];
            previous = [NSString stringWithString: possible];
        } else
            break;
        possible = [self evaluateMathomaticString: @"#+1"];
    }
    
    return nil;
}

// If a string is returned, it is displayed as an error
- (NSString*)prepareWithDelegate:(MathomaticViewController*)c
{
    NSArray * recentExpressions = [c recentExpressions:YES unique: YES];
    
    if ([recentExpressions count] == 0)
        return @"Please enter one or more equations (ie. a=b) before attempting to solve.";
    
    controller = [c retain];
    EquationSelectController * select = [[EquationSelectController alloc] initWithEquations:recentExpressions andOperation:self];
    [c presentModalViewController:select animated:YES];
    [select release];
    
    return nil;
}

- (void)equationsSelected:(NSArray*)equations solveVariable:(NSString*)var
{
    inputs = [[NSMutableArray alloc] init];
    [inputs addObjectsFromArray: equations];
    variable = [var retain];
    
    [controller performCommand: self];
    [controller release];
    controller = nil;
}

@end
