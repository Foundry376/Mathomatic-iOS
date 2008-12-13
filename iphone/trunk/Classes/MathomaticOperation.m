//
//  MathomaticOperation.m
//  mathomatic
//
//  Created by Ben Gotow on 11/20/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "MathomaticOperation.h"
#import "MathomaticExpression.h"
#import "Mathomatic.h"

@implementation MathomaticOperation
@synthesize inputs;
@synthesize steps;
@synthesize stepNames;
@synthesize name;

- (id)init
{
    if (self = [super init]){
        inputs = [[NSMutableArray alloc] init];
        steps = [[NSMutableArray alloc] init];
        stepNames = [[NSMutableArray alloc] init];
    }
    return self;
}

- (id)initWithCoder:(NSCoder *)decoder
{
    if (self = [super init]){
        self.inputs = [decoder decodeObject];
        self.steps = [decoder decodeObject];
        self.stepNames = [decoder decodeObject];
        self.name = [decoder decodeObject];
    }
    return self;
}

- (void)encodeWithCoder:(NSCoder *)encoder
{
    [encoder encodeObject: inputs];
    [encoder encodeObject: steps];
    [encoder encodeObject: stepNames];
    [encoder encodeObject: name];
}

- (void)performMathomaticSetup
{
}

// If a string is returned, it is displayed as an error
- (NSString*)perform
{
    MathomaticExpression * input = [inputs lastObject];
    
    [steps removeAllObjects];
    [stepNames removeAllObjects];
    
    // unload all of the expressions from mathomatic, so don't accidentally solve using expressions
    // we don't want!
    clear_all();
    
    // if the expression contains no variables and is not an equation, we want to approximate it's value.
    // This is done by prefixing the expression with a=, and then approximating and removing the left hand
    // side. 
    if ([[input mathomaticText] rangeOfCharacterFromSet:[NSCharacterSet characterSetWithCharactersInString:@"xyz="]].location == NSNotFound)
    {
        NSString * equation = [NSString stringWithFormat: @"a=%@", [input mathomaticText]];
        [self evaluateMathomaticString: equation];
        
        NSString * result = [self evaluateMathomaticString: @"approximate"];
        result = [result stringByReplacingOccurrencesOfString:@" " withString:@""];
        
        // get the right hand side of the result (to chop of the a=)
        MathomaticExpression * rhs = [[MathomaticExpression expressionWithMathomaticText: result] rhs];
        result = [rhs mathomaticText];
        
        // check to see if the result is a simple fraction that we should display as a decimal.
        NSArray * components = [result componentsSeparatedByString: @"/"];
        if (([components count] == 2) && ([result rangeOfCharacterFromSet: [NSCharacterSet characterSetWithCharactersInString:@"*+-^pixyz="]].location == NSNotFound))
            result = [[NSNumber numberWithFloat: [[components objectAtIndex:0] floatValue] / [[components objectAtIndex:1] floatValue]] stringValue];
        
        // Add the result to our inputs
        if (![result isEqualToString: @"Equation space is empty."]){
            [self addInputMathomaticString: result];
            [inputs removeObject: input];
        }
    } else {
        [self evaluateMathomaticString: [input mathomaticText]];
    }
    return nil;
}

// If a string is returned, it is displayed as an error
- (NSString*)prepareWithDelegate:(MathomaticViewController*)controller
{
    // subclasses use this function to display any necessary sheets or selectors. Once they've loaded all
    // the necessary information, they call [controller performCommand: self] to invoke perform.
    return nil;
}

#pragma mark Convenience Functions for Subclasses

- (void)addInput:(MathomaticExpression*)expression
{
    [inputs addObject: expression];
}

- (void)addInputMathomaticString:(NSString*)s
{
    [self addInput: [MathomaticExpression expressionWithMathomaticText: s]];
}

- (void)addStepMathomaticString:(NSString*)mathomaticText withName:(NSString*)n
{
    [self addStep:[MathomaticExpression expressionWithMathomaticText:mathomaticText] withName:n];
}

- (void)addStep:(MathomaticExpression*)expression withName:(NSString*)n
{
    int ii = [stepNames indexOfObject: n];
    if (ii == NSNotFound){
        [steps addObject: [NSMutableArray arrayWithObject: expression]];
        [stepNames addObject: n];
    } else {
        [[steps objectAtIndex: ii] addObject: expression];
    }
}

- (NSString*)evaluateExpression:(MathomaticExpression*)i
{
    char * buffer = NULL;
    matho_process([[i mathomaticText] cStringUsingEncoding: NSStringEncodingConversionAllowLossy], &buffer);
    if (buffer != NULL){
        NSString * result = [NSString stringWithCString: buffer];
        free(buffer);
        return result;
    }else
        return nil;
}

- (NSString*)evaluateMathomaticString:(NSString*)i
{
    return [self evaluateExpression: [MathomaticExpression expressionWithMathomaticText: i]];
}

- (void)dealloc
{
    [inputs release];
    [steps release];
    [stepNames release];
    [controller release];
    [name release];
    [super dealloc];
}


@end
