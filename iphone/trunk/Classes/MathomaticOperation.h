//
//  MathomaticOperation.h
//  mathomatic
//
//  Created by Ben Gotow on 11/20/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "MathomaticExpression.h"
#import "MathomaticViewController.h"

@interface MathomaticOperation : NSObject <NSCoding> {
    
    NSMutableArray * inputs;
    NSMutableArray * steps;
    NSMutableArray * stepNames;
    MathomaticViewController * controller;
    NSString * name;
}

@property (nonatomic, retain) NSMutableArray * inputs;
@property (nonatomic, retain) NSMutableArray * steps;
@property (nonatomic, retain) NSMutableArray * stepNames;
@property (nonatomic, retain) NSString * name;

- (id)init;

- (id)initWithCoder:(NSCoder *)decoder;
- (void)encodeWithCoder:(NSCoder *)encoder;

- (void)addInput:(MathomaticExpression*)expression;
- (void)addInputMathomaticString:(NSString*)s;
- (void)addStep:(MathomaticExpression*)expression withName:(NSString*)n;
- (void)addStepMathomaticString:(NSString*)mathomaticText withName:(NSString*)n;

- (NSString*)evaluateExpression:(MathomaticExpression*)i;
- (NSString*)evaluateMathomaticString:(NSString*)i;

- (NSString*)perform;
- (void)performMathomaticSetup;
- (NSString*)prepareWithDelegate:(MathomaticViewController*)controller;

@end
