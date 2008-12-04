//
//  MathomaticExpression.h
//  mathomatic
//
//  Created by Ben Gotow on 11/20/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface MathomaticExpression : NSObject <NSCoding> {

    NSString        * mathomaticText;
    NSString        * equationText;
    UIImage         * equationImage;
    
    BOOL              optimizeEquationImageParenthesis;
}

@property (nonatomic, retain, setter=setMathomaticText:) NSString * mathomaticText;
@property (nonatomic, retain, setter=setEquationText:) NSString * equationText;
@property (nonatomic, retain, readonly, getter=equationImage) UIImage * equationImage;

+ expressionWithMathomaticText:(NSString*)text;
+ expressionWithEquationText:(NSString*)text;
- (id)initWithCoder:(NSCoder *)decoder;
- (void)encodeWithCoder:(NSCoder *)encoder;
- (void)setMathomaticText:(NSString*)t;
- (void)setEquationText:(NSString*)t;
- (BOOL)isEquation;
- (BOOL)isValidExpression;
- (NSArray*)equationVariables;
- (UIImage*)equationImage;

- (MathomaticExpression*)lhs;
- (MathomaticExpression*)rhs;
@end
