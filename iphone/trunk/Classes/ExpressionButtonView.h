//
//  ExpressionButtonView.h
//  mathomatic
//
//  Created by Ben Gotow on 11/26/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "MathomaticExpression.h"

@interface ExpressionButtonView : UIControl {
    MathomaticExpression * expression;
    UIImageView * expressionImageView;
    BOOL showTip;
}

@property (nonatomic, retain, setter=setExpression:) MathomaticExpression * expression;
@property (nonatomic, retain) UIImageView * expressionImageView;


- (id)initWithExpression:(MathomaticExpression*)e;
- (id)initWithFrame:(CGRect)frame;
- (void)setExpression:(MathomaticExpression*)e;
- (void)dealloc;
- (void)fitToFrame:(CGRect)frame;
- (void)drawRect:(CGRect)rect;

@end
