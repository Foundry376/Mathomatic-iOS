//
//  MathomaticOperationTableViewCell.h
//  mathomatic
//
//  Created by Ben Gotow on 11/20/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "MathomaticExpression.h"

@class MathomaticOperation;

@protocol MathomaticOperationTableViewCellDelegate
- (void)mathomaticOperationTableViewCellClicked:(MathomaticExpression*)expression;
@end

@interface MathomaticOperationTableViewCell : UITableViewCell {

    MathomaticOperation * operation;
    int height;
    id<MathomaticOperationTableViewCellDelegate> delegate;
}

@property (nonatomic, retain, setter=setOperation:) MathomaticOperation * operation;
@property (nonatomic, assign) int height;
@property (nonatomic, retain) id<MathomaticOperationTableViewCellDelegate> delegate;

- (id)initWithFrame:(CGRect)frame reuseIdentifier:(NSString *)reuseIdentifier delegate:(id<MathomaticOperationTableViewCellDelegate>)d;
- (void)setOperation:(MathomaticOperation*)op;

- (void)attachText:(NSString*)text withSize:(int)size andColor:(UIColor*)color;
- (void)attachEquations:(NSArray*)equations;
- (void)setSelected:(BOOL)selected animated:(BOOL)animated;
- (void)dealloc;

@end
