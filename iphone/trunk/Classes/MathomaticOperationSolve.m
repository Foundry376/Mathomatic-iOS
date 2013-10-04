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
        variable = nil;
    }
    return self;
}

// If a string is returned, it is displayed as an error
- (NSString*)perform
{
    NSMutableArray * variables = [NSMutableArray array];
    
    [self performMathomaticSetup];
    
    NSMutableArray * remainingExpressions = [NSMutableArray arrayWithArray: inputs];
    MathomaticExpression * finalExpression = nil;
    MathomaticExpression * substitutionExpression = nil;
    
    // find an expression that we will substitute all the others into. This expression needs
    // to contain the variable we want to solve for - at least, that makes it easier.
    for (MathomaticExpression * ex in remainingExpressions){
        if (([[ex equationVariables] containsObject: variable]) && (finalExpression == nil))
            finalExpression = ex;
    
        // compile a master list of all the variables in the equation
        for (NSString * var in [ex equationVariables])
            if (![variables containsObject: var])
                [variables addObject: var];
    }
    
    // if there are other variables and other expressions, perform substitutions.  
    if (([remainingExpressions count] > 1) && ([variables count] > 1)){
        
        // remove the variable we're solving for. We don't have to perform any substitutions with that variable,
        // because we make sure it's in the final expression above.
        [variables removeObject: variable];
        
        // iterate through the remaining variables. We will perform substitutions for each of these.
        for (NSString * var in variables){
            // find an expression containing that variable. Remove it from the remainingExpressions list.
            for (MathomaticExpression * ex in remainingExpressions){
                if ([[ex equationVariables] containsObject: var] && (ex != finalExpression)){
                    substitutionExpression = ex;
                    break;
                }
            }
            [remainingExpressions removeObject: substitutionExpression];
            
            // for each of the remainingExpressions, use the substitutionExpression to remove var
            for (int ii = [remainingExpressions count]-1; ii >= 0; ii--){
                MathomaticExpression * ex = [remainingExpressions objectAtIndex: ii];
                [self evaluateMathomaticString:@"clear all"];
                [self evaluateExpression: substitutionExpression];
                [self evaluateExpression: ex];
                
                NSString * result = [self evaluateMathomaticString: [NSString stringWithFormat: @"eliminate %@", var]];
                
                // verify the result and add a step to the output representing it. We have to be careful to modify the existing mathomaticExpression
                // so that the reference to finalExpression is still valid at the end.
                if (([result isEqualToString: @"No substitutions made."] != YES) && ([result isEqualToString: @"Variable not found."] != YES)){
                    result = [result stringByReplacingOccurrencesOfString:@" " withString:@""];
                    [self addStepMathomaticString:result withName: [NSString stringWithFormat: @"Eliminating %@", [var uppercaseString]]];
                    
                    [ex setMathomaticText: result];
                }
            }
        }
    }
    
    // we should now be left with remainingExpressions count = 1 (finalExpression).
    [self evaluateMathomaticString: @"clear all"];
    [self evaluateExpression: finalExpression];
    NSString * result = [self evaluateMathomaticString: [NSString stringWithFormat: @"solve %@", variable]];
    if (result == nil)
        return @"The system could not be solved.";
    
    // print out the solution in expression form
    [self addStepMathomaticString: result withName:[NSString stringWithFormat: @"Solving for %@", [variable uppercaseString]]];
    
    // if the solution contained a sign, expand the sign. This fills equation spaces ahead of us.
    NSString * previous = nil;
    NSString * possible = nil;
    if ([result rangeOfString: @"sign"].location != NSNotFound){
        [self evaluateMathomaticString: @"simplify sign"];
        possible = [self evaluateMathomaticString: @"#+1"];
    }
    
    // keep advancing forward, approximating and reading values until two are the same. That indicates
    // that we've reached the end of the list of approximated results.
    while (true){
        possible = [self evaluateMathomaticString: @"approximate"];
        if ((![possible isEqualToString: previous]) && (![possible isEqualToString:@"Equation space is empty."]) && (![possible isEqualToString: result])){
            [self addStepMathomaticString: [possible stringByReplacingOccurrencesOfString:@" " withString:@""] withName:@"Solutions"];
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
    [c presentViewController:select animated:YES completion:NULL];
    [select release];
    
    return nil;
}

- (void)equationSelectCancelled
{
    [self release];
}

- (void)equationsSelected:(NSArray*)equations solveVariable:(NSString*)var
{
    [inputs addObjectsFromArray: equations];
    variable = [var retain];
    
    [controller performCommand: self];
    [controller release];
    controller = nil;
}

- (void)dealloc
{
    [variable release];
    [super dealloc];
}

@end
