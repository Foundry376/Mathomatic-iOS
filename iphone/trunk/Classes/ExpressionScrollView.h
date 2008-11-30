//
//  ExpressionScrollView.h
//  mathomatic
//
//  Created by Ben Gotow on 11/26/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "MathomaticExpression.h"

@interface ExpressionScrollView : UIScrollView {
    MathomaticExpression * expression;
    UIImageView * expressionImageView;
}

@property (nonatomic, retain, setter=setExpression:) MathomaticExpression * expression;
@property (nonatomic, retain) UIImageView * expressionImageView;

- (id)awakeFromNib;
- (void)setExpression:(MathomaticExpression*)e;
- (void)dealloc;
- (void)fitToFrame:(CGRect)frame;

@end
