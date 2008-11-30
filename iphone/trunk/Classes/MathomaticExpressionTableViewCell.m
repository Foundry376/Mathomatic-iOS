//
//  MathomaticOperationTableViewCell.m
//  mathomatic
//
//  Created by Ben Gotow on 11/20/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "MathomaticExpressionTableViewCell.h"


@implementation MathomaticExpressionTableViewCell

@synthesize expression;

- (id)initWithFrame:(CGRect)frame reuseIdentifier:(NSString *)reuseIdentifier {
    if (self = [super initWithFrame:frame reuseIdentifier:reuseIdentifier]) {
        expression = nil;
        expressionImageView = [[UIImageView alloc] initWithFrame: CGRectZero];
        [self addSubview: expressionImageView];
    }
    return self;
}

- (void)setExpression:(MathomaticExpression*)exp
{
    [expression release];
    expression = [exp retain];
    [expressionImageView setImage: [exp equationImage]];
    
    CGSize size = [[exp equationImage] size];
    CGSize padding = CGSizeMake(18, 6);
    
    float scaleFactor = fmin(1, (294-padding.width*2) / size.width);
    
    [expressionImageView setFrame: CGRectMake(padding.width, padding.height, size.width * scaleFactor, size.height * scaleFactor)];
}

- (int)height
{
    return [expressionImageView frame].size.height + [expressionImageView frame].origin.y * 2;
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated 
{
    // Configure the view for the selected state
}

- (void)dealloc {
    [expressionImageView release];
    [expression release];
    [super dealloc];
}


@end
