//
//  MathomaticOperationTableViewCell.h
//  mathomatic
//
//  Created by Ben Gotow on 11/20/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "MathomaticExpression.h"

@interface MathomaticExpressionTableViewCell : UITableViewCell {

    MathomaticExpression * expression;
    UIImageView *          expressionImageView;
}

@property (nonatomic, retain, setter=setExpression:) MathomaticExpression * expression;

- (id)initWithFrame:(CGRect)frame reuseIdentifier:(NSString *)reuseIdentifier;
- (int)height;
- (void)setExpression:(MathomaticExpression*)exp;
- (void)setSelected:(BOOL)selected animated:(BOOL)animated;
- (void)dealloc;

@end
