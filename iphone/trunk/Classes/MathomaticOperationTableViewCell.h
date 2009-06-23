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
    BOOL contentShifted;
    
    NSObject<MathomaticOperationTableViewCellDelegate>* delegate;
}

@property (nonatomic, retain, setter=setOperation:) MathomaticOperation * operation;
@property (nonatomic, assign) int height;
@property (nonatomic, retain) NSObject<MathomaticOperationTableViewCellDelegate>* delegate;

- (id)initWithReuseIdentifier:(NSString *)reuseIdentifier delegate:(NSObject<MathomaticOperationTableViewCellDelegate>*)d;
- (void)setOperation:(MathomaticOperation*)op;

- (void)attachText:(NSString*)text withSize:(int)size andColor:(UIColor*)color;
- (void)attachEquations:(NSArray*)equations;
- (void)setSelected:(BOOL)selected animated:(BOOL)animated;
- (void)dealloc;

@end
