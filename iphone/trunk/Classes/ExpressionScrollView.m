//
//  EquationView.m
//  mathomatic
//
//  Created by Ben Gotow on 11/13/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "ExpressionScrollView.h"

@implementation ExpressionScrollView

@synthesize expression;
@synthesize expressionImageView;

- (id)awakeFromNib
{
    [self setClipsToBounds: NO];
    [self setOpaque: NO];
    [self setBackgroundColor: [UIColor clearColor]];
    [self setShowsHorizontalScrollIndicator:YES];
    [self setShowsVerticalScrollIndicator: NO];
    [self setUserInteractionEnabled: YES];
    
    expressionImageView = [[UIImageView alloc] initWithFrame: [self frame]];
    [self addSubview: expressionImageView];
    
    return self;
}

- (void)setExpression:(MathomaticExpression*)e
{
    [expression release];
    expression = [e retain];
    [expressionImageView setImage: [expression equationImage]];
    [self fitToFrame: [self frame]];
}

- (void)dealloc 
{
    [expression release];
    [expressionImageView release];
    [super dealloc];
}

- (void)fitToFrame:(CGRect)frame
{
    CGSize max = frame.size;
    CGSize content = [[expressionImageView image] size];
    CGSize padding = CGSizeMake(8,4);
    
    if (max.height == 0)
        max.height = content.height + padding.height * 2;
    
    float scaleFactor = fminf(1, (max.height - padding.height * 2) / content.height);
    CGRect content_fit = CGRectMake(padding.width, 
                                    padding.height + (max.height - padding.height * 2 - content.height * scaleFactor)/2,
                                    content.width * scaleFactor, 
                                    content.height * scaleFactor);
    
    [expressionImageView setFrame: content_fit];

    content_fit.size.width += padding.width*2;
    [self setContentSize: content_fit.size];
    [self scrollRectToVisible:CGRectMake(content_fit.size.width-10, 0, 10, 10) animated:YES];
}

@end
